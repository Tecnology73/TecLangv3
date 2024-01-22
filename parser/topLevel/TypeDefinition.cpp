#include "TypeDefinition.h"

#include "../../compiler/Compiler.h"
#include "../../symbolTable/SymbolTable.h"
#include "../../ast/expressions/TypeReference.h"
#include "../expressions/Expression.h"
#include "../expressions/TypeReference.h"

namespace {
    TypeField* parseField(Parser* parser) {
        auto isPublic = parser->currentToken.is(Token::Type::Public);
        auto isPrivate = parser->currentToken.is(Token::Type::Private);
        if (parser->currentToken.is(Token::Type::Public, Token::Type::Private))
            parser->NextToken(); // Consume 'public' or 'private'

        // TODO: Would it be confusing allowing keywords to have different meanings based on the context?
        // Instead of explicitly listing "type" to be allowed here, we should probably tokenize
        // things differently so that we can allow other keywords.
        if (parser->currentToken.isNot(Token::Type::Identifier, Token::Type::Type)) {
            parser->PrintSyntaxError("field name");
            return nullptr;
        }

        auto field = new TypeField(
            parser->currentToken,
            StringInternTable::Intern(parser->currentToken.value)
        );
        if (isPublic) field->flags.Set(TypeFlag::PUBLIC);
        else if (isPrivate) field->flags.Set(TypeFlag::PRIVATE);

        if (parser->NextToken().is(Token::Type::Colon)) {
            parser->NextToken(); // Consume ':'

            field->type = parseTypeReference(parser);
        } else
            field->type = TypeReference::Infer();

        if (parser->currentToken.is(Token::Type::Assign)) {
            parser->NextToken(); // Consume '='
            field->expression = parseExpression(parser);
            if (!field->expression)
                return nullptr;
        }

        return field;
    }
}

TypeDefinition* parseTypeDefinition(Parser* parser) {
    auto isPublic = parser->currentToken.is(Token::Type::Public);
    auto isPrivate = parser->currentToken.is(Token::Type::Private);
    if (parser->currentToken.is(Token::Type::Public, Token::Type::Private))
        parser->NextToken(); // Consume 'public' or 'private'

    if (parser->currentToken.isNot(Token::Type::Type)) {
        parser->PrintSyntaxError("type");
        return nullptr;
    }

    if (parser->PeekToken().isNot(Token::Type::Identifier)) {
        parser->PrintSyntaxError("type name");
        return nullptr;
    }

    const auto typeDef = TypeDefinition::Create(parser->NextToken());
    if (isPublic) typeDef->flags.Set(TypeFlag::PUBLIC);
    else if (isPrivate) typeDef->flags.Set(TypeFlag::PRIVATE);

    SymbolTable::GetInstance()->Add(typeDef);

    if (parser->NextToken().isNot(Token::Type::OpenCurly)) {
        parser->PrintSyntaxError("{");
        return nullptr;
    }

    parser->NextToken(); // Consume '{'
    while (parser->currentToken.isNot(Token::Type::CloseCurly)) {
        const auto field = parseField(parser);
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
