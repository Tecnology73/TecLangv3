#pragma once

#include "../Parser.h"
#include "../expressions/Expression.h"

bool parseBlock(Parser *parser, std::vector<Node *> &block) {
    if (parser->currentToken.isNot(Token::Type::OpenCurly)) {
        parser->PrintSyntaxError("{");
        return false;
    }

    parser->NextToken(); // Consume '{'
    while (parser->currentToken.isNot(Token::Type::CloseCurly)) {
        auto expression = parseExpression(parser);
        if (expression == nullptr) return false;

        block.push_back(expression);
    }

    if (parser->currentToken.isNot(Token::Type::CloseCurly)) {
        parser->PrintSyntaxError("}");
        return false;
    }

    parser->NextToken(); // Consumes '}'
    return true;
}