#pragma once

#include "../../ast/Literals.h"
#include "../Parser.h"

String *parseString(Parser *parser) {
    if (parser->currentToken.isNot(Token::Type::String)) {
        parser->PrintSyntaxError("string literal (e.g. \"Forty-two\")");
        return nullptr;
    }

    auto node = new String(parser->currentToken);
    node->value = parser->currentToken.value.substr(1, parser->currentToken.value.length() - 2);

    parser->NextToken();
    return node;
}