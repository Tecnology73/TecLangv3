#include "Enum.h"
#include "../expressions/Expression.h"
#include "../statements/FunctionParameter.h"
#include "../../ast/topLevel/TypeDefinition.h"

namespace ParseEnum {
    EnumValue* parseValue(Parser* parser) {
        if (parser->currentToken.isNot(Token::Type::Identifier)) {
            parser->PrintSyntaxError("enum value name");
            return nullptr;
        }

        auto value = new EnumValue(
            parser->currentToken,
            StringInternTable::Intern(parser->currentToken.value)
        );
        if (parser->NextToken().is(Token::Type::Assign)) {
            parser->NextToken(); // Consume '='

            value->expression = parseExpression(parser);
            if (!value->expression)
                return nullptr;
        }

        return value;
    }

    EnumConstructor* parseConstructor(Parser* parser) {
        if (parser->currentToken.isNot(Token::Type::Identifier)) {
            parser->PrintSyntaxError("enum value name");
            return nullptr;
        }

        auto constructor = new EnumConstructor(
            parser->currentToken,
            StringInternTable::Intern(parser->currentToken.value)
        );
        if (parser->NextToken().isNot(Token::Type::OpenParen)) {
            parser->PrintSyntaxError("(");
            return nullptr;
        }

        // Parse parameters
        parser->NextToken();
        // v4(i32, i32, i32, i32)
        // v4(a: i32, b: i32, c: i32, d: i32)
        while (parser->currentToken.isNot(Token::Type::CloseParen)) {
            if (!parseFunctionParameter(parser, constructor, true))
                return nullptr;

            if (parser->currentToken.is(Token::Type::Comma))
                parser->NextToken(); // Consume ':'
            else if (parser->currentToken.isNot(Token::Type::CloseParen)) {
                parser->PrintSyntaxError("',' or ')'");
                return nullptr;
            }
        }

        parser->NextToken(); // Consume ')'
        return constructor;
    }
}

Enum* parseEnum(Parser* parser) {
    if (parser->currentToken.isNot(Token::Type::Enum)) {
        parser->PrintSyntaxError("enum");
        return nullptr;
    }

    if (parser->PeekToken().isNot(Token::Type::Identifier)) {
        parser->PrintSyntaxError("enum name");
        return nullptr;
    }

    auto anEnum = Enum::Create(
        parser->currentToken,
        StringInternTable::Intern(parser->NextToken().value)
    );
    if (parser->NextToken().isNot(Token::Type::OpenCurly)) {
        parser->PrintSyntaxError("{");
        return nullptr;
    }

    // Parse values
    parser->NextToken();
    while (parser->currentToken.isNot(Token::Type::CloseCurly)) {
        if (parser->currentToken.isNot(Token::Type::Identifier)) {
            parser->PrintSyntaxError("argument identifier");
            return nullptr;
        }

        if (parser->PeekToken().is(Token::Type::OpenParen)) {
            auto constructor = ParseEnum::parseConstructor(parser);
            if (!constructor)
                return nullptr;

            constructor->returnType = anEnum->CreateConstructorType(constructor);
            anEnum->AddFunction(constructor);
        } else {
            auto value = ParseEnum::parseValue(parser);
            if (!value)
                return nullptr;

            value->type = anEnum->CreateReference();
            anEnum->AddField(value);
        }
    }

    if (parser->currentToken.isNot(Token::Type::CloseCurly)) {
        parser->PrintSyntaxError("}");
        return nullptr;
    }

    parser->NextToken(); // Consume '}'
    return anEnum;
}
