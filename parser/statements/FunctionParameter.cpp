#include "FunctionParameter.h"
#include "../../ast/topLevel/TypeDefinition.h"

bool parseFunctionParameter(Parser* parser, Function* function) {
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

    // TODO: Add default value support.
    // TODO: Infer type from default value.
    function->AddParameter(
        beginToken,
        StringInternTable::Intern(beginToken.value),
        new TypeReference(
            parser->currentToken,
            StringInternTable::Intern(parser->currentToken.value)
        )
    );
    parser->NextToken(); // Consume the type name.
    return true;
}
