#pragma once

#include <string>
#include <vector>
#include "../lexer/Lexer.h"
#include "../ast/Node.h"
#include "../compiler/ErrorManager.h"

class Parser {
public:
    explicit Parser(Lexer* lexer);

    bool Parse();

    Token NextToken();

    Token PeekToken();

    void PrintSyntaxError(const std::string& args) const {
        ErrorManager::Report(ErrorCode::SYNTAX_UNEXPECTED_TOKEN, {currentToken.value.data(), args}, this, currentToken);
    }

    void PrintError(
        const ErrorCode errorCode,
        const std::vector<std::string>& args,
        const Token& token
    ) const {
        ErrorManager::Report(errorCode, args, this, token);
    }

    Node* GetNextNode() {
        if (astLoopIndex >= ast.size()) return nullptr;
        return ast[astLoopIndex++];
    }

    unsigned GetAstLoopIndex() const {
        return astLoopIndex;
    }

    void IncAstLoop() {
        astLoopIndex++;
    }

    void DecAstLoop() {
        astLoopIndex--;
    }

    void ResetAstLoop() {
        astLoopIndex = 0;
    }

    void SetAstNode(const unsigned index, Node* node) {
        if (index < ast.size()) return;
        ast[index] = node;
    }

    static Parser* FromSource(const std::string& source);

    static Parser* FromFile(const std::string& path);

public:
    Token currentToken;
    Token peekToken;

    // private:
    Lexer* lexer;
    std::vector<Node *> ast;

private:
    unsigned astLoopIndex = 0;
};
