#include "VariableDeclaration.h"
#include "../expressions/Expression.h"
#include "../expressions/TypeReference.h"
#include "../../ast/StringInternTable.h"

VariableDeclaration* parseVariableDeclaration(Parser* parser) {
    if (parser->currentToken.isNot(Token::Type::Identifier, Token::Type::Type)) {
        parser->PrintSyntaxError("variable name");
        return nullptr;
    }

    auto node = new VariableDeclaration(
        parser->currentToken,
        StringInternTable::Intern(parser->currentToken.value)
    );
    if (parser->PeekToken().is(Token::Type::Colon)) {
        parser->NextToken(); // Consume ':'
        if (parser->NextToken().isNot(Token::Type::Identifier, Token::Type::Type)) {
            parser->PrintSyntaxError("type");
            return nullptr;
        }

        node->type = parseTypeReference(parser);
    } else if (parser->PeekToken().isNot(Token::Type::Assign)) {
        parser->PrintSyntaxError(": or =");
        return nullptr;
    }

    if (parser->currentToken.is(Token::Type::Assign)) {
        parser->NextToken(); // Consume '='
        node->expression = parseExpression(parser, 1);
    }

    return node;
}
