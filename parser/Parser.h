#pragma once

#include <string>
#include <vector>
#include <variant>
#include "../lexer/Lexer.h"
#include "../ast/Node.h"
#include "../compiler/ErrorManager.h"

class Parser {
public:
    explicit Parser(Lexer *lexer);

    bool Parse();

    Token NextToken();

    Token PeekToken();

    void PrintSyntaxError(const std::string &args) const {
        ErrorManager::Report(ErrorCode::SYNTAX_UNEXPECTED_TOKEN, {currentToken.value, args}, this, currentToken);
    }

    void PrintError(
        ErrorCode errorCode,
        const std::vector<std::string> &args,
        const Token &token
    ) const {
        ErrorManager::Report(errorCode, args, this, token);
    }

    static Parser *FromSource(const std::string &source);

    static Parser *FromFile(const std::string &path);

public:
    Token currentToken;
    Token peekToken;

    // private:
    Lexer *lexer;
    std::vector<Node *> ast;
};
