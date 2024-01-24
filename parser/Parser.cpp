#include "Parser.h"
#include <format>
#include "../App.h"
#include "expressions/Function.h"
#include "topLevel/TypeDefinition.h"
#include "topLevel/Enum.h"
#include "topLevel/ExternFunction.h"

Parser::Parser(Lexer* lexer) {
    this->lexer = lexer;
}

bool Parser::Parse() {
    NextToken();
    while (!lexer->IsAtEnd()) {
        if (currentToken.is(Token::Type::EndOfFile)) break;

        Node* node = nullptr;
        if (currentToken.is(Token::Type::Function)) {
            auto fn = parseFunction(this);
            // Does commenting this out break anything?
            // Yes... It fucks up the entire flow of when types & their functions are generated.
            if (fn && fn->ownerType) continue;

            node = fn;
        } else if (currentToken.is(Token::Type::Type, Token::Type::Public, Token::Type::Private)) {
            node = parseTypeDefinition(this);
        } else if (currentToken.is(Token::Type::Enum)) {
            node = parseEnum(this);
        } else if (currentToken.is(Token::Type::Extern)) {
            node = parseExternFunction(this);
        } else {
            // idk why I wrote this... Because why not I guess?
            std::string hintWord;
            switch (currentToken.value[0]) {
                case 'e':
                    if (currentToken.value.length() < 2)
                        hintWord = "'extern' or 'enum'";
                    if (currentToken.value[1] == 'x')
                        hintWord = "'extern'";
                    else if (currentToken.value[1] == 'n')
                        hintWord = "'enum'";
                    break;
                case 'f':
                    hintWord = "'func'";
                    break;
                case 't':
                    hintWord = "'type'";
                    break;
                case 'p':
                    hintWord = "'pub'";
                    break;
                case 'r':
                    hintWord = "'priv'";
                    break;
                case 'i':
                    hintWord = "'import'";
                    break;
            }

            if (!hintWord.empty())
                ErrorManager::QueueHint("Did you mean {}?", {hintWord});

            ErrorManager::Report(ErrorCode::SYNTAX_ERROR, {std::string(currentToken.value)}, this, currentToken);
            return false;
        }

        if (!node) return false;

        ast.push_back(node);
    }

    return true;
}

Token Parser::NextToken() {
    currentToken = lexer->GetNextToken();
    peekToken.type = Token::Type::Unknown;

    return currentToken;
}

Token Parser::PeekToken() {
    if (peekToken.type != Token::Type::Unknown) return peekToken;
    return peekToken = lexer->PeekToken();
}

Parser* Parser::FromSource(const std::string& source) {
    return new Parser(Lexer::FromSource(source));
}

Parser* Parser::FromFile(const std::string& path) {
    auto lexer = Lexer::FromFile(path);
    if (!lexer) return nullptr;

    return new Parser(lexer);
}
