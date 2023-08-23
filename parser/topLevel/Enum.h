#pragma once

#include "../../ast/TopLevel.h"
#include "../Parser.h"
#include "../expressions/Expression.h"

namespace {
    EnumValue *parseEnumValue(Parser *parser) {
        if (parser->currentToken.isNot(Token::Type::Identifier)) {
            parser->PrintSyntaxError("enum value name");
            return nullptr;
        }

        auto value = new EnumValue(parser->currentToken);
        value->name = parser->currentToken.value;

        if (parser->NextToken().is(Token::Type::Assign)) {
            parser->NextToken(); // Consume '='

            value->expression = parseExpression(parser);
            if (!value->expression)
                return nullptr;
        }

        return value;
    }

    EnumConstructor *parseEnumConstructor(Parser *parser) {
        if (parser->currentToken.isNot(Token::Type::Identifier)) {
            parser->PrintSyntaxError("enum value name");
            return nullptr;
        }

        auto constructor = new EnumConstructor(parser->currentToken);
        constructor->name = parser->currentToken.value;
        if (parser->NextToken().isNot(Token::Type::OpenParen)) {
            parser->PrintSyntaxError("(");
            return nullptr;
        }

        // Parse parameters
        parser->NextToken();
        // v4(i32, i32, i32, i32)
        // v4(a: i32, b: i32, c: i32, d: i32)
        while (parser->currentToken.isNot(Token::Type::CloseParen)) {
            auto argument = new EnumParameter(parser->currentToken);
            if (parser->PeekToken().is(Token::Type::Colon)) {
                // ID: TypeName
                argument->name = parser->currentToken.value;
                parser->NextToken(); // Consume ':'
                argument->type = TypeDefinition::Create(parser->NextToken());
            } else {
                // TypeName
                argument->type = TypeDefinition::Create(parser->currentToken);
            }

            constructor->parameters.push_back(argument);

            if (parser->NextToken().is(Token::Type::Comma))
                parser->NextToken(); // Consume ','
        }

        parser->NextToken(); // Consume ')'
        return constructor;
    }
}

Enum *parseEnum(Parser *parser) {
    if (parser->currentToken.isNot(Token::Type::Enum)) {
        parser->PrintSyntaxError("enum");
        return nullptr;
    }

    if (parser->NextToken().isNot(Token::Type::Identifier)) {
        parser->PrintSyntaxError("enum name");
        return nullptr;
    }

    auto anEnum = new Enum(parser->currentToken);
    anEnum->name = parser->currentToken.value;

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
            auto constructor = parseEnumConstructor(parser);
            if (!constructor)
                return nullptr;

            anEnum->AddValue(constructor);
        } else {
            auto value = parseEnumValue(parser);
            if (!value)
                return nullptr;

            anEnum->AddValue(value);
        }
    }

    if (parser->currentToken.isNot(Token::Type::CloseCurly)) {
        parser->PrintSyntaxError("}");
        return nullptr;
    }

    parser->NextToken(); // Consume '}'
    return anEnum;
}