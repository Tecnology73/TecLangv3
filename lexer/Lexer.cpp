#include <map>
#include <utility>
#include <charconv>
#include <fstream>
#include <sstream>
#include "Lexer.h"
#include "PunctuationTrie.h"
#include "util.h"

const std::map<std::string, Token::Type> keywords = {
    {"func",     Token::Type::Function},
    {"if",       Token::Type::If},
    {"else",     Token::Type::Else},
    {"return",   Token::Type::Return},
    {"true",     Token::Type::Boolean_True},
    {"false",    Token::Type::Boolean_False},
    {"for",      Token::Type::For},
    {"continue", Token::Type::Continue},
    {"break",    Token::Type::Break},
    {"in",       Token::Type::In},
    {"type",     Token::Type::Type},
    {"new",      Token::Type::New},
    {"when",     Token::Type::When},
    {"extern",   Token::Type::Extern},
    {"as",       Token::Type::As},
    {"step",     Token::Type::Step},
    {"enum",     Token::Type::Enum},
};

const auto punctuationTrie = new PunctuationTrie(
    {
        {"(",   Token::Type::OpenParen},
        {")",   Token::Type::CloseParen},
        {"{",   Token::Type::OpenCurly},
        {"}",   Token::Type::CloseCurly},
        {"[",   Token::Type::OpenBracket},
        {"]",   Token::Type::CloseBracket},
        {",",   Token::Type::Comma},
        {":",   Token::Type::Colon},
        {"::",  Token::Type::ColonColon},
        {";",   Token::Type::Semicolon},
        {".",   Token::Type::Period},
        {"..",  Token::Type::Spread},
        {"...", Token::Type::TripleDots},
        {"=",   Token::Type::Assign},
        {"+",   Token::Type::Plus},
        {"++",  Token::Type::PlusPlus},
        {"-",   Token::Type::Minus},
        {"--",  Token::Type::MinusMinus},
        {"*",   Token::Type::Multiply},
        {"/",   Token::Type::Divide},
        {"\\",  Token::Type::BackSlash},
        {"%",   Token::Type::Percent},
        {"&",   Token::Type::Ampersand},
        {"|",   Token::Type::Pipe},
        {"^",   Token::Type::Caret},
        {"!",   Token::Type::Exclamation},
        {"~",   Token::Type::Tilde},
        {"?",   Token::Type::Question},
        {"<",   Token::Type::LessThan},
        {">",   Token::Type::GreaterThan},
        {"<=",  Token::Type::LessThanOrEqual},
        {">=",  Token::Type::GreaterThanOrEqual},
        {"==",  Token::Type::Equal},
        {"!=",  Token::Type::NotEqual},
        {"&&",  Token::Type::LogicalAnd},
        {"||",  Token::Type::LogicalOr},
        {"+=",  Token::Type::PlusEqual},
        {"-=",  Token::Type::MinusEqual},
        {"*=",  Token::Type::MultiplyEqual},
        {"/=",  Token::Type::DivideEqual},
        {"%=",  Token::Type::PercentEqual},
        {"&=",  Token::Type::AmpersandEqual},
        {"^=",  Token::Type::CaretEqual},
        {"|=",  Token::Type::PipeEqual},
        {"->",  Token::Type::Arrow},
    }
);

Lexer::Lexer(const std::string &source) {
    this->source = source;
    sourceLength = (long) source.length();

    // Split the source into lines.
    /*std::stringstream ss(source);
    std::string line;
    while (std::getline(ss, line, '\n'))
        lines.push_back(line);*/
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
        else if (isdigit(c)) return parseNumber();
        else if (c == '"') return parseString();
        else if (ispunct(c)) {
            char n = peekChar();
            if (c == '/' && n == '/') {
                parseLineComment();
                continue;
            } else if (c == '/' && n == '*') {
                parseBlockComment();
                continue;
            }

            return parsePunctuation();
        }

        return makeToken(Token::Type::Unknown, std::string(1, c), position);
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

std::vector<std::string> Lexer::GetSurroundingCode(Position startPos, Position endPos, long &topHalfIndex) {
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
    auto pos = position;
    std::string identifier;

    char c;
    while (c = peekChar(0), c != EOF && isalnum(c))
        identifier += consumeChar();

    auto type = Token::Type::Identifier;
    if (keywords.count(identifier) > 0)
        type = keywords.at(identifier);

    return makeToken(type, std::move(identifier), pos);
}

Token Lexer::parseNumber() {
    auto pos = position;

    // Parse either an integer or a double.
    // If the number contains a decimal point, it's a double.
    // Otherwise, it's an integer.
    bool isDouble = false;
    std::string number;
    char c = peekChar(0);

    // Only allow signs ('-' or '+') at the start of the number.
    if ((c == '-' || c == '+') && isdigit(peekChar(1))) {
        if (position.index == 0 || !isdigit(peekChar(-1))) {
            number += consumeChar();
        } else
            return parsePunctuation();
    }

    while (c = peekChar(0), c != EOF && (isdigit(c) || c == '.' || c == '_')) {
        if (c == '_') {
            consumeChar();
            continue;
        }

        // TODO: Don't allow multiple decimal points.
        if (c == '.') {
            // Spread operator. e.g. 1..10
            if (peekChar() == '.') break;
            isDouble = true;
        }

        number += consumeChar();
    }

    if (isDouble) {
        double value;
        auto result = std::from_chars(
            number.data(),
            number.data() + number.length(),
            value
        );

        if (result.ec == std::errc())
            return makeToken(Token::Type::Double, value, pos);
    } else {
        int value;
        auto result = std::from_chars(
            number.data(),
            number.data() + number.length(),
            value
        );

        if (result.ec == std::errc())
            return makeToken(Token::Type::Integer, value, pos);
    }

    return makeToken(Token::Type::Unknown, std::move(number), pos);
}

Token Lexer::parseString() {
    auto pos = position;
    std::string string;
    string += consumeChar();

    while (true) {
        char c = peekChar(0);
        if (c == EOF) break;

        if (c == '"') {
            string += consumeChar();
            break;
        }

        if (c == '\\') {
            char n = peekChar();
            switch (n) {
                case 'n':
                    string += '\n';
                    break;
                case 't':
                    string += '\t';
                    break;
                case 'r':
                    string += '\r';
                    break;
                case '0':
                    string += '\0';
                    break;
                case 'b':
                    string += '\b';
                    break;
                case 'f':
                    string += '\f';
                    break;
                case 'v':
                    string += '\v';
                    break;
                case '\'':
                    string += '\'';
                    break;
                case '"':
                    string += '"';
                    break;
                case '\\':
                    string += '\\';
                    break;
                default:
                    string += c;
                    string += n;
                    break;
            }

            consumeChar();
            consumeChar();
        } else {
            string += consumeChar();
        }
    }

    return makeToken(Token::Type::String, std::move(string), pos);
}

Token Lexer::parsePunctuation() {
    // Signed numbers.
    char c = peekChar(0);
    char nc = peekChar(1);
    if ((c == '-' || c == '+' || c == '.') && isdigit(nc) && !isdigit(peekChar(-1)))
        return parseNumber();

    std::string punctuation;
    TrieNode *curr = punctuationTrie->getRoot();
    while (curr != nullptr && curr->children.count(peekChar(0)) > 0) {
        char cc = consumeChar();
        punctuation += cc;
        curr = curr->children[cc];
    }

    // Create the token based on the punctuation
    Token::Type tokenType = (curr != nullptr) ? curr->tokenType : Token::Type::Punctuation;
    return makeToken(tokenType, std::move(punctuation), position);
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

Token Lexer::makeToken(Token::Type type, std::string value, Position pos) const {
    Token token;
    token.type = type;
    token.position = pos;
    token.position.length = position.index - pos.index;
    token.value = std::move(value);

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

Lexer *Lexer::FromSource(const std::string &source) {
    return new Lexer(source);
}

Lexer *Lexer::FromFile(const std::string &path) {
    std::ifstream t(path);

    t.seekg(0, std::ios::end);

    auto size = t.tellg();
    auto source = std::string(size, ' ');

    t.seekg(0);
    t.read(&source[0], size);

    return new Lexer(source, path);
}
