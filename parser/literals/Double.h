#pragma once

#include "../../ast/Literals.h"
#include "../Parser.h"

Double *parseDouble(Parser *parser) {
    if (parser->currentToken.isNot(Token::Type::Double)) {
        parser->PrintSyntaxError("double literal (e.g. 42.0)");
        return nullptr;
    }

    auto node = new Double(parser->currentToken);
    node->value = parser->currentToken.doubleValue;

    parser->NextToken();
    return node;
}