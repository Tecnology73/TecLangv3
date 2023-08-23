#pragma once

#include "../Parser.h"
#include "../../ast/Statements.h"

Continue *parseContinue(Parser *parser) {
    if (parser->currentToken.isNot(Token::Type::Continue)) {
        parser->PrintSyntaxError("continue");
        return nullptr;
    }

    auto node = new Continue(parser->currentToken);
    parser->NextToken(); // Consume 'continue'

    return node;
}