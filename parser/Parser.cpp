#include "Parser.h"
#include <format>
#include "expressions/Function.h"
#include "statements/TypeDefinition.h"
#include "topLevel/Enum.h"

Parser::Parser(Lexer *lexer) {
    this->lexer = lexer;
}

bool Parser::Parse() {
    NextToken();
    while (!lexer->IsAtEnd()) {
        if (currentToken.is(Token::Type::EndOfFile)) break;

        Node *node = nullptr;
        if (currentToken.type == Token::Type::Function || currentToken.type == Token::Type::Extern) {
            auto fn = parseFunction(this);
            if (fn && fn->ownerType) continue;

            node = fn;
        } else if (currentToken.type == Token::Type::Type) {
            node = parseTypeDefinition(this);
        } else if (currentToken.type == Token::Type::Enum) {
            node = parseEnum(this);
        }

        if (node == nullptr) return false;

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

Parser *Parser::FromSource(const std::string &source) {
    return new Parser(Lexer::FromSource(source));
}

Parser *Parser::FromFile(const std::string &path) {
    return new Parser(Lexer::FromFile(path));
}