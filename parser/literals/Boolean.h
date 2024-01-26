#pragma once

#include "../Parser.h"
#include "../../ast/literals/Boolean.h"

Boolean* parseBoolean(Parser* parser) {
    if (parser->currentToken.isNot(Token::Type::Boolean_True, Token::Type::Boolean_False)) {
        parser->PrintSyntaxError("boolean literal (e.g. true/false)");
        return nullptr;
    }

    auto node = new Boolean(parser->currentToken);
    node->value = parser->currentToken.is(Token::Type::Boolean_True);

    parser->NextToken();
    return node;
}
