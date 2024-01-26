#pragma once

#include "../Parser.h"
#include "../../ast/literals/Null.h"

Null* parseNull(Parser* parser) {
    if (parser->currentToken.isNot(Token::Type::Null)) {
        parser->PrintSyntaxError("null");
        return nullptr;
    }

    auto node = new Null(parser->currentToken);

    parser->NextToken();
    return node;
}
