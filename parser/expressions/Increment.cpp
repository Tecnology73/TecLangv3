#include "Increment.h"
#include "../../ast/Literals.h"
#include "../../ast/Expressions.h"
#include "Expression.h"

Node *parseIncrement(Parser *parser) {
    if (
        parser->currentToken.isNot(Token::Type::PlusPlus) &&
        parser->currentToken.isNot(Token::Type::MinusMinus)
        ) {
        parser->PrintSyntaxError("++ or --");
        return nullptr;
    }

    auto node = new Integer(parser->currentToken);
    node->value = 1;
    parser->NextToken(); // Consume '++' or '--'

    // Most likely a suffix/postfix increment/decrement.
    // In this case, the identifier has already been parsed.
    if (parser->currentToken.isNot(Token::Type::Identifier))
        return node;

    // Prefix increment/decrement.
    auto rhs = parseExpression(parser);
    if (!rhs) {
        parser->PrintSyntaxError("expression");
        return nullptr;
    }

    return BinaryOperation::Create(node->token, node, rhs);
}