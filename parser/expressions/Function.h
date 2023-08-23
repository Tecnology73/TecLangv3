#pragma once

#include "../../ast/Statements.h"
#include "../Parser.h"
#include "Expression.h"
#include "../statements/FunctionParameter.h"

Function *parseFunction(Parser *parser) {
    bool isExternal = parser->currentToken.is(Token::Type::Extern);
    if (isExternal)
        parser->NextToken(); // Consume 'extern'

    if (parser->currentToken.isNot(Token::Type::Function)) {
        parser->PrintSyntaxError("func");
        return nullptr;
    }

    // Function belongs to a type.
    TypeDefinition *ownerType = nullptr;
    if (!isExternal && parser->PeekToken().is(Token::Type::LessThan)) {
        parser->NextToken();

        if (parser->NextToken().isNot(Token::Type::Identifier)) {
            parser->PrintSyntaxError("type");
            return nullptr;
        }

        ownerType = TypeDefinition::Create(parser->currentToken);
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

    auto function = new Function(parser->currentToken);
    function->name = parser->NextToken().value;
    function->ownerType = ownerType;
    function->isExternal = isExternal;

    if (ownerType) {
        ownerType->addFunction(function);

        // "this" is always the first parameter.
        // Constructors have "this" malloc'd.
        if (function->name != "construct") {
            auto thisDecl = new FunctionParameter(function->token);
            thisDecl->function = function;
            thisDecl->name = "this";
            thisDecl->type = ownerType;
            function->addParameter(thisDecl);
        }
    }

    if (parser->NextToken().isNot(Token::Type::OpenParen)) {
        parser->PrintSyntaxError("(");
        return nullptr;
    }

    // Parse parameters
    parser->NextToken();
    while (parser->currentToken.isNot(Token::Type::CloseParen)) {
        if (parser->currentToken.isNot(Token::Type::Identifier)) {
            parser->PrintSyntaxError("argument identifier");
            return nullptr;
        }

        auto parameter = parseFunctionParameter(parser);
        if (!parameter)
            return nullptr;

        function->addParameter(parameter);

        if (parser->currentToken.is(Token::Type::Comma)) {
            parser->NextToken();
        } else if (parser->currentToken.isNot(Token::Type::CloseParen)) {
            parser->PrintSyntaxError(",  or )");
            return nullptr;
        }
    }

    if (parser->currentToken.isNot(Token::Type::CloseParen)) {
        parser->PrintSyntaxError(")");
        return nullptr;
    }

    // The return type could be omitted when it needs to be inferred.
    if (parser->NextToken().is(Token::Type::Identifier)) {
        auto name = parser->currentToken;
        if (parser->NextToken().is(Token::Type::Multiply)) {
            name.value += "*";
            parser->NextToken();
        }

        function->returnType = TypeDefinition::Create(name);
    }

    if (!function->isExternal) {
        // If the return type is omitted and this is a constructor, the return type is the owner type.
        if (!function->returnType && function->ownerType && function->name == "construct")
            function->returnType = function->ownerType;

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