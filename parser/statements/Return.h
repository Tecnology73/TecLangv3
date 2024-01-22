#pragma once

#include "../../ast/Statements.h"
#include "../Parser.h"
#include "../expressions/Expression.h"

Return *parseReturn(Parser *parser) {
    if (parser->currentToken.isNot(Token::Type::Return)) {
        parser->PrintSyntaxError("return");
        return nullptr;
    }

    auto node = new Return(parser->currentToken);
    parser->NextToken();

    node->expression = parseExpression(parser);
    if (node->expression == nullptr)
        return nullptr;

    return node;
}
