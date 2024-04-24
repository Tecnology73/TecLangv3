#include "FunctionParameter.h"
#include "../../ast/topLevel/TypeDefinition.h"
#include "../../ast/StringInternTable.h"
#include "../expressions/TypeReference.h"

bool parseFunctionParameter(Parser* parser, Function* function, const bool isNameOptional) {
    if (parser->currentToken.isNot(Token::Type::Identifier)) {
        if (isNameOptional)
            parser->PrintSyntaxError("param name or type");
        else
            parser->PrintSyntaxError("parameter name");

        return false;
    }

    auto beginToken = parser->currentToken;
    auto hasName = false;
    if (!isNameOptional || parser->PeekToken().is(Token::Type::Colon)) {
        if (parser->NextToken().isNot(Token::Type::Colon)) {
            parser->PrintSyntaxError(":");
            return false;
        }

        if (parser->NextToken().isNot(Token::Type::Identifier)) {
            parser->PrintSyntaxError("type");
            return false;
        }

        hasName = true;
    }

    // TODO: Add default value support.
    // TODO: Infer type from default value.
    auto paramType = parseTypeReference(parser);
    if (!paramType) {
        parser->PrintSyntaxError("type");
        return false;
    }

    if (hasName)
        function->AddParameter(
            beginToken,
            StringInternTable::Intern(beginToken.value),
            paramType
        );
    else
        function->AddParameter(beginToken, paramType);
    
    return true;
}
