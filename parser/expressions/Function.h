#pragma once

#include "../../ast/Statements.h"
#include "../Parser.h"
#include "Expression.h"
#include "TypeReference.h"
#include "../statements/FunctionParameter.h"
#include "../../compiler/Compiler.h"
#include "../../context/SymbolTable.h"

Function* parseFunction(Parser* parser) {
    auto isExternal = parser->currentToken.is(Token::Type::Extern);
    if (isExternal)
        parser->NextToken(); // Consume 'extern'

    if (parser->currentToken.isNot(Token::Type::Function)) {
        parser->PrintSyntaxError("func");
        return nullptr;
    }

    // Function belongs to a type.
    TypeBase* ownerType = nullptr;
    if (!isExternal && parser->PeekToken().is(Token::Type::LessThan)) {
        parser->NextToken();

        if (parser->NextToken().isNot(Token::Type::Identifier)) {
            parser->PrintSyntaxError("type");
            return nullptr;
        }

        // ownerType = parseTypeReference(parser);
        // if (parser->currentToken.isNot(Token::Type::GreaterThan)) {
        ownerType = TypeDefinition::CreateUndeclared(parser->currentToken);
        if (parser->NextToken().isNot(Token::Type::GreaterThan)) {
            parser->PrintSyntaxError(">");
            return nullptr;
        }
    }

    // Peek so we can reference the token where the statement starts.
    if (parser->PeekToken().isNot(Token::Type::Identifier)) {
        parser->PrintSyntaxError("statement name");
        return nullptr;
    }

    auto function = new Function(parser->currentToken, parser->NextToken().value);
    function->ownerType = ownerType;
    function->isExternal = isExternal;

    if (ownerType) {
        ownerType->AddFunction(function);

        // "this" is always the first parameter.
        // Constructors have "this" malloc'd.
        if (function->name != "construct")
            function->AddParameter(parser->currentToken, "this", ownerType->CreateReference());
    } else
        SymbolTable::GetInstance()->Add(function);

    if (parser->NextToken().isNot(Token::Type::OpenParen)) {
        parser->PrintSyntaxError("(");
        return nullptr;
    }

    // Parse parameters
    parser->NextToken();
    while (parser->currentToken.isNot(Token::Type::CloseParen)) {
        if (!parseFunctionParameter(parser, function))
            return nullptr;

        if (parser->currentToken.is(Token::Type::Comma)) {
            parser->NextToken(); // Consume ','
        } else if (parser->currentToken.isNot(Token::Type::CloseParen)) {
            parser->PrintSyntaxError(", or )");
            return nullptr;
        }
    }

    if (parser->currentToken.isNot(Token::Type::CloseParen)) {
        parser->PrintSyntaxError(")");
        return nullptr;
    }

    // The return type could be omitted when it needs to be inferred.
    if (parser->NextToken().is(Token::Type::Identifier))
        function->returnType = parseTypeReference(parser);

    if (!function->isExternal) {
        // If the return type is omitted and this is a constructor, the return type is the owner type.
        if (!function->returnType && function->ownerType && function->name == "construct")
            function->returnType = function->ownerType->CreateReference();

        if (parser->currentToken.isNot(Token::Type::OpenCurly)) {
            parser->PrintSyntaxError("{'");
            return nullptr;
        }

        parser->NextToken(); // Consume '{'
        while (parser->currentToken.isNot(Token::Type::CloseCurly)) {
            auto expression = parseExpression(parser);
            if (expression == nullptr) return nullptr;

            function->body.push_back(expression);
        }
    }

    if (!isExternal) {
        if (parser->currentToken.isNot(Token::Type::CloseCurly)) {
            parser->PrintSyntaxError("}");
            return nullptr;
        }

        parser->NextToken(); // Consume '}'
    }

    return function;
}
