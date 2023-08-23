#include "VariableDeclaration.h"
#include "../expressions/Expression.h"

VariableDeclaration *parseVariableDeclaration(Parser *parser) {
    if (parser->currentToken.isNot(Token::Type::Identifier)) {
        parser->PrintSyntaxError("variable name");
        return nullptr;
    }

    auto node = new VariableDeclaration(parser->currentToken);
    node->name = parser->currentToken.value;

    if (parser->PeekToken().is(Token::Type::Colon)) {
        parser->NextToken(); // Consume ':'
        if (parser->NextToken().isNot(Token::Type::Identifier)) {
            parser->PrintSyntaxError("type");
            return nullptr;
        }

        node->type = TypeDefinition::Create(parser->currentToken);
    } else if (parser->PeekToken().isNot(Token::Type::Assign)) {
        parser->PrintSyntaxError(": or =");
        return nullptr;
    }

    if (parser->NextToken().is(Token::Type::Assign)) {
        parser->NextToken(); // Consume '='
        node->expression = parseExpression(parser, 1);
    }

    return node;
}