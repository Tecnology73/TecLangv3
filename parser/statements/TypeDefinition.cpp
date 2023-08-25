#include "TypeDefinition.h"
#include "../expressions/Expression.h"

namespace {
    TypeField *parseField(Parser *parser) {
        if (parser->currentToken.isNot(Token::Type::Identifier)) {
            parser->PrintSyntaxError("field name");
            return nullptr;
        }

        auto field = new TypeField(parser->currentToken, parser->currentToken.value);
        if (parser->NextToken().is(Token::Type::Colon)) {
            parser->NextToken(); // Consume ':'

            field->type = TypeDefinition::Create(parser->currentToken);
            parser->NextToken(); // Consume type
        }

        if (parser->currentToken.is(Token::Type::Assign)) {
            parser->NextToken(); // Consume '='
            field->expression = parseExpression(parser);
            if (!field->expression)
                return nullptr;
        }

        return field;
    }
}

TypeDefinition *parseTypeDefinition(Parser *parser) {
    if (parser->currentToken.isNot(Token::Type::Type)) {
        parser->PrintSyntaxError("type");
        return nullptr;
    }

    if (parser->PeekToken().isNot(Token::Type::Identifier)) {
        parser->PrintSyntaxError("type name");
        return nullptr;
    }

    auto typeDef = TypeDefinition::Create(parser->NextToken());
    if (parser->NextToken().isNot(Token::Type::OpenCurly)) {
        parser->PrintSyntaxError("{");
        return nullptr;
    }

    parser->NextToken(); // Consume '{'
    while (parser->currentToken.isNot(Token::Type::CloseCurly)) {
        auto field = parseField(parser);
        if (!field)
            return nullptr;

        typeDef->AddField(field);
    }

    if (parser->currentToken.isNot(Token::Type::CloseCurly)) {
        parser->PrintSyntaxError("}");
        return nullptr;
    }

    parser->NextToken(); // Consume '}'
    return typeDef;
}
