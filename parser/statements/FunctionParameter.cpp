#include "FunctionParameter.h"
#include "../../ast/topLevel/TypeDefinition.h"
#include "../../ast/StringInternTable.h"

bool parseFunctionParameter(Parser* parser, Function* function, const bool isNameOptional) {
    if (parser->currentToken.isNot(Token::Type::Identifier)) {
        if (isNameOptional)
            parser->PrintSyntaxError("param name or type");
        else
            parser->PrintSyntaxError("parameter name");

        return false;
    }

    auto beginToken = parser->currentToken;
    if (!isNameOptional) {
        if (parser->NextToken().isNot(Token::Type::Colon)) {
            parser->PrintSyntaxError(":");
            return false;
        }

        if (parser->NextToken().isNot(Token::Type::Identifier)) {
            parser->PrintSyntaxError("type");
            return false;
        }
    }

    // TODO: Add default value support.
    // TODO: Infer type from default value.
    auto paramType = new TypeReference(
        parser->currentToken,
        StringInternTable::Intern(parser->currentToken.value)
    );
    if (isNameOptional)
        function->AddParameter(beginToken, paramType);
    else
        function->AddParameter(
            beginToken,
            StringInternTable::Intern(beginToken.value),
            paramType
        );

    parser->NextToken(); // Consume the param/type name.
    return true;
}
