#include "FunctionParameter.h"

FunctionParameter *parseFunctionParameter(Parser *parser) {
    if (parser->currentToken.isNot(Token::Type::Identifier)) {
        parser->PrintSyntaxError("argument name");
        return nullptr;
    }

    auto parameter = new FunctionParameter(parser->currentToken);
    parameter->name = parser->currentToken.value;

    if (parser->NextToken().isNot(Token::Type::Colon)) {
        parser->PrintSyntaxError(":");
        return nullptr;
    }

    if (parser->NextToken().isNot(Token::Type::Identifier)) {
        parser->PrintSyntaxError("type");
        return nullptr;
    }

    parameter->type = TypeDefinition::Create(parser->currentToken);

    parser->NextToken(); // Consume the type name.
    return parameter;
}