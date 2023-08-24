#include "../../ast/Expressions.h"
#include "../../ast/Literals.h"
#include "Expression.h"

// TODO: IS THIS ACTUALLY CORRECT!?!?
Node *parseIncrement(Parser *parser) {
    if (
        parser->currentToken.isNot(Token::Type::PlusPlus) &&
        parser->currentToken.isNot(Token::Type::MinusMinus)
        ) {
        parser->PrintSyntaxError("++ or --");
        return nullptr;
    }

    auto lhs = new Integer(parser->currentToken);
    lhs->value = 1;
    parser->NextToken(); // Consume '++' or '--'

    // Most likely a suffix/postfix increment/decrement.
    // In this case, the identifier has already been parsed.
    if (parser->currentToken.isNot(Token::Type::Identifier))
        return lhs;

    // Prefix increment/decrement.
    auto rhs = parseExpression(parser);
    if (!rhs) {
        parser->PrintSyntaxError("expression");
        return nullptr;
    }

    return new BinaryOperation(lhs->token, lhs, rhs);
}