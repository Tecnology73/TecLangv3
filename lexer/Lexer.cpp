#include <utility>
#include <charconv>
#include <fstream>
#include <sstream>
#include <iostream>
#include "Lexer.h"
#include "PunctuationTrie.h"
#include "keywords_hash.h"

const auto punctuationTrie = new PunctuationTrie(
    {
        {"(", Token::Type::OpenParen},
        {")", Token::Type::CloseParen},
        {"{", Token::Type::OpenCurly},
        {"}", Token::Type::CloseCurly},
        {"[", Token::Type::OpenBracket},
        {"]", Token::Type::CloseBracket},
        {",", Token::Type::Comma},
        {":", Token::Type::Colon},
        {"::", Token::Type::ColonColon},
        {";", Token::Type::Semicolon},
        {".", Token::Type::Period},
        {"..", Token::Type::Spread},
        {"...", Token::Type::TripleDots},
        {"=", Token::Type::Assign},
        {"+", Token::Type::Plus},
        {"++", Token::Type::PlusPlus},
        {"-", Token::Type::Minus},
        {"--", Token::Type::MinusMinus},
        {"*", Token::Type::Multiply},
        {"/", Token::Type::Divide},
        {"\\", Token::Type::BackSlash},
        {"%", Token::Type::Percent},
        {"&", Token::Type::Ampersand},
        {"|", Token::Type::Pipe},
        {"^", Token::Type::Caret},
        {"!", Token::Type::Exclamation},
        {"~", Token::Type::Tilde},
        {"?", Token::Type::Question},
        {"<", Token::Type::LessThan},
        {">", Token::Type::GreaterThan},
        {"<=", Token::Type::LessThanOrEqual},
        {">=", Token::Type::GreaterThanOrEqual},
        {"==", Token::Type::Equal},
        {"!=", Token::Type::NotEqual},
        {"&&", Token::Type::LogicalAnd},
        {"||", Token::Type::LogicalOr},
        {"+=", Token::Type::PlusEqual},
        {"-=", Token::Type::MinusEqual},
        {"*=", Token::Type::MultiplyEqual},
        {"/=", Token::Type::DivideEqual},
        {"%=", Token::Type::PercentEqual},
        {"&=", Token::Type::AmpersandEqual},
        {"^=", Token::Type::CaretEqual},
        {"|=", Token::Type::PipeEqual},
        {"->", Token::Type::Arrow},
    }
);

Lexer::Lexer(const std::string& source) {
    this->source = source;
    sourceLength = static_cast<long>(this->source.length());

    // Split the source into lines.
    std::stringstream ss(source);
    std::string line;
    while (std::getline(ss, line, '\n'))
        lines.push_back(line);
}

Token Lexer::GetNextToken() {
    while (position.index < this->sourceLength) {
        char c = peekChar(0);
        if (c == EOF) break;

        if (isspace(c)) {
            consumeChar();
            continue;
        }

        if (isalpha(c)) return parseIdentifier();
        if (isdigit(c)) return parseNumber();
        if (c == '"') return parseString();

        if (ispunct(c)) {
            char n = peekChar();
            if (c == '/' && n == '/') {
                parseLineComment();
                continue;
            }

            if (c == '/' && n == '*') {
                parseBlockComment();
                continue;
            }

            return parsePunctuation();
        }

        return makeToken(
            Token::Type::Unknown,
            std::string_view(source.data() + position.index, 1),
            position
        );
    }

    Token token;
    token.type = Token::Type::EndOfFile;
    token.position = position;

    return token;
}

Token Lexer::PeekToken() {
    auto pos = this->position;
    auto token = GetNextToken();

    this->position = pos;

    return token;
}

bool Lexer::IsAtEnd() const {
    return position.index >= (long) this->sourceLength;
}

size_t Lexer::CountLines() const {
    return lines.size();
}

std::vector<std::string> Lexer::GetSurroundingCode(Position startPos, Position endPos, long& topHalfIndex) {
    constexpr int NUM_LINES = 2; // How many extra lines to get before/after the provided positions.
    long startLine = std::max(0l, startPos.line - NUM_LINES);
    long endLine = std::min((long) lines.size(), endPos.line + 1 + NUM_LINES);

    topHalfIndex = startPos.line - startLine;

    auto startIt = lines.begin() + startLine;
    auto endIt = lines.begin() + endLine;

    std::vector<std::string> code(startIt, endIt);
    return code;
}

char Lexer::consumeChar() {
    if (position.index >= this->sourceLength) return EOF;

    char c = source[position.index++];
    if (c == '\n') {
        position.line++;
        position.column = 0;
    } else {
        position.column++;
    }

    return c;
}

char Lexer::peekChar(int offset) const {
    if (position.index + offset >= this->sourceLength) return EOF;
    return source[position.index + offset];
}

Token Lexer::parseIdentifier() {
    auto start = position;

    char c;
    while (c = peekChar(0), c != EOF && isalnum(c))
        consumeChar();

    auto identifier = std::string_view(source.data() + start.index, position.index - start.index);
    auto type = Token::Type::Identifier;
    auto keyword = Perfect_Hash::in_word_set(identifier);
    if (keyword != nullptr)
        type = keyword->type;

    return makeToken(type, identifier, start);
}

Token Lexer::parseNumber() {
    auto start = position;

    // Parse either an integer or a double.
    // If the number contains a decimal point, it's a double.
    // Otherwise, it's an integer.
    bool isDouble = false;
    char c = peekChar(0);

    // Only allow signs ('-' or '+') at the start of the number.
    if ((c == '-' || c == '+') && isdigit(peekChar(1))) {
        if (position.index == 0 || !isdigit(peekChar(-1))) {
            consumeChar();
        } else
            return parsePunctuation();
    }

    while (c = peekChar(0), c != EOF && (isdigit(c) || c == '.' || c == '_')) {
        if (c == '_') {
            consumeChar();
            continue;
        }

        if (c == '.') {
            // Spread operator. e.g. 1..10
            if (peekChar() == '.') break;
            // Don't allow more than one decimal point.
            if (isDouble) break;
            isDouble = true;
        }

        consumeChar();
    }

    if (isDouble) {
        double value;
        auto result = std::from_chars(
            source.data() + start.index,
            source.data() + position.index,
            value
        );

        if (result.ec == std::errc())
            return makeToken(Token::Type::Double, value, start);
    } else {
        int value;
        auto result = std::from_chars(
            source.data() + start.index,
            source.data() + position.index,
            value
        );

        if (result.ec == std::errc())
            return makeToken(Token::Type::Integer, value, start);
    }

    return makeToken(
        Token::Type::Unknown,
        std::string_view(source.data() + start.index, position.index - start.index),
        start
    );
}

Token Lexer::parseString() {
    auto start = position;

    while (true) {
        char c = peekChar(0);
        if (c == EOF) break;

        consumeChar();

        if (c == '"') break;
    }

    return makeToken(
        Token::Type::String,
        std::string_view(source.data() + start.index, position.index - start.index),
        start
    );
}

Token Lexer::parsePunctuation() {
    // Signed numbers.
    char c = peekChar(0);
    char nc = peekChar(1);
    if ((c == '-' || c == '+' || c == '.') && isdigit(nc) && !isdigit(peekChar(-1)))
        return parseNumber();

    auto start = position;
    auto curr = punctuationTrie->getRoot();
    while (curr != nullptr && curr->children.contains(peekChar(0)))
        curr = curr->children[consumeChar()];

    // Create the token based on the punctuation
    Token::Type tokenType = (curr != nullptr) ? curr->tokenType : Token::Type::Punctuation;
    return makeToken(
        tokenType,
        std::string_view(source.data() + start.index, position.index - start.index),
        position
    );
}

void Lexer::parseLineComment() {
    char c;
    while (c = peekChar(0), c != EOF && c != '\n')
        consumeChar();
}

void Lexer::parseBlockComment() {
    char c;
    while (c = peekChar(0), c != EOF) {
        consumeChar();

        if (c == '*' && peekChar(0) == '/') {
            consumeChar();
            break;
        }
    }
}

Token Lexer::makeToken(Token::Type type, std::string_view value, Position pos) const {
    Token token;
    token.type = type;
    token.position = pos;
    token.position.length = position.index - pos.index;
    token.value = value;

    return token;
}

Token Lexer::makeToken(Token::Type type, int value, Position pos) const {
    Token token;
    token.type = type;
    token.position = pos;
    token.position.length = position.index - pos.index;
    token.intValue = value;

    return token;
}

Token Lexer::makeToken(Token::Type type, double value, Position pos) const {
    Token token;
    token.type = type;
    token.position = pos;
    token.position.length = position.index - pos.index;
    token.doubleValue = value;

    return token;
}

Lexer* Lexer::FromSource(const std::string& source) {
    return new Lexer(source);
}

Lexer* Lexer::FromFile(const std::string& path) {
    std::ifstream t(path);
    if (!t.is_open()) {
        std::cerr << "Failed to open file '" << path << "'" << std::endl;
        return nullptr;
    }

    t.seekg(0, std::ios::end);

    auto size = t.tellg();
    auto source = std::string(size, ' ');

    t.seekg(0);
    t.read(&source[0], size);

    return new Lexer(source, path);
}
