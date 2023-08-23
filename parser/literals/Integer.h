#pragma once

#include "../../ast/Literals.h"
#include "../Parser.h"

Integer *parseInteger(Parser *parser) {
    if (parser->currentToken.isNot(Token::Type::Integer)) {
        parser->PrintSyntaxError("integer literal (e.g. 42)");
        return nullptr;
    }

    auto node = new Integer(parser->currentToken, parser->currentToken.intValue);

    parser->NextToken();
    return node;
}