#include "FunctionParameter.h"

bool parseFunctionParameter(Parser *parser, Function *function) {
    if (parser->currentToken.isNot(Token::Type::Identifier)) {
        parser->PrintSyntaxError("parameter name");
        return false;
    }

    auto beginToken = parser->currentToken;
    if (parser->NextToken().isNot(Token::Type::Colon)) {
        parser->PrintSyntaxError(":");
        return false;
    }

    if (parser->NextToken().isNot(Token::Type::Identifier)) {
        parser->PrintSyntaxError("type");
        return false;
    }

    // function->addParameter(beginToken, beginToken.value, TypeDefinition::Create(parser->currentToken));
    function->addParameter(beginToken, beginToken.value, nullptr);
    parser->NextToken(); // Consume the type name.
    return true;
}