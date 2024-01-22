#pragma once

#include "../../ast/TopLevel.h"
#include "../Parser.h"
#include "../expressions/Expression.h"
#include "../../symbolTable/SymbolTable.h"
#include "../expressions/TypeReference.h"

namespace {
    EnumValue* parseEnumValue(Parser* parser) {
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

    EnumConstructor* parseEnumConstructor(Parser* parser) {
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
            if (parser->PeekToken().is(Token::Type::Colon)) {
                // id: Type
                auto param = new EnumParameter(
                    parser->currentToken,
                    StringInternTable::Intern(parser->currentToken.value)
                );

                parser->NextToken(); // Consume identifier

                if (parser->NextToken().isNot(Token::Type::Identifier)) {
                    parser->PrintSyntaxError("type");
                    return nullptr;
                }

                param->type = parseTypeReference(parser);
                constructor->parameters.push_back(param);
            } else {
                // Type
                auto param = new EnumParameter(parser->currentToken);
                param->type = parseTypeReference(parser);
            }

            if (parser->NextToken().is(Token::Type::Comma))
                parser->NextToken(); // Consume ','
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
    SymbolTable::GetInstance()->Add(anEnum);
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

            anEnum->AddField(constructor);
        } else {
            auto value = parseEnumValue(parser);
            if (!value)
                return nullptr;

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
