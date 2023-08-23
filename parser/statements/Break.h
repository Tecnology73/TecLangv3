#pragma once

#include "../Parser.h"
#include "../../ast/Statements.h"

Break *parseBreak(Parser *parser) {
    if (parser->currentToken.isNot(Token::Type::Break)) {
        parser->PrintSyntaxError("break");
        return nullptr;
    }

    auto node = new Break(parser->currentToken);
    parser->NextToken(); // Consume 'break'

    return node;
}