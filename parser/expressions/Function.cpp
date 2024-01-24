#include "Function.h"
#include "../Parser.h"
#include "Expression.h"
#include "TypeReference.h"
#include "../statements/FunctionParameter.h"
#include "../../compiler/Compiler.h"
#include "../../symbolTable/SymbolTable.h"
#include "../../ast/StringInternTable.h"

bool parseParameters(Parser* parser, Function* function, const bool isParamNameOptional) {
    if (parser->currentToken.isNot(Token::Type::OpenParen)) {
        parser->PrintSyntaxError("'('");
        return false;
    }

    // Parse parameters
    parser->NextToken();
    while (parser->currentToken.isNot(Token::Type::CloseParen)) {
        if (!parseFunctionParameter(parser, function, isParamNameOptional))
            return false;

        if (parser->currentToken.is(Token::Type::Comma)) {
            parser->NextToken(); // Consume ','
        } else if (parser->currentToken.isNot(Token::Type::CloseParen)) {
            parser->PrintSyntaxError("',' or ')'");
            return false;
        }
    }

    if (parser->currentToken.isNot(Token::Type::CloseParen)) {
        parser->PrintSyntaxError("')'");
        return false;
    }

    parser->NextToken(); // Consume ')'
    return true;
}

Function* parseFunction(Parser* parser) {
    if (parser->currentToken.isNot(Token::Type::Function)) {
        parser->PrintSyntaxError("func");
        return nullptr;
    }

    // Function belongs to a type.
    TypeBase* ownerType = nullptr;
    if (parser->PeekToken().is(Token::Type::LessThan)) {
        parser->NextToken();

        if (parser->NextToken().isNot(Token::Type::Identifier)) {
            parser->PrintSyntaxError("type");
            return nullptr;
        }

        ownerType = TypeDefinition::CreateUndeclared(parser->currentToken);
        if (parser->NextToken().isNot(Token::Type::GreaterThan)) {
            parser->PrintSyntaxError("'>'");
            return nullptr;
        }
    }

    // Peek so we can reference the token where the statement starts.
    if (parser->PeekToken().isNot(Token::Type::Identifier)) {
        parser->NextToken(); // Consume 'func'
        parser->PrintSyntaxError("function name");
        return nullptr;
    }

    auto function = new Function(
        parser->currentToken,
        StringInternTable::Intern(parser->NextToken().value)
    );
    function->ownerType = ownerType;

    if (ownerType) {
        ownerType->AddFunction(function);

        // "this" is always the first parameter.
        // Constructors have "this" malloc'd.
        if (function->name != "construct")
            function->AddParameter(
                parser->currentToken,
                StringInternTable::Intern("this"),
                ownerType->CreateReference()
            );
    } else
        SymbolTable::GetInstance()->Add(function);

    parser->NextToken(); // Consume func name
    if (!parseParameters(parser, function))
        return nullptr;

    // The return type could be omitted when it needs to be inferred.
    if (parser->currentToken.is(Token::Type::Identifier))
        function->returnType = parseTypeReference(parser);

    // If the return type is omitted and this is a constructor, the return type is the owner type.
    if (!function->returnType && function->ownerType && function->name == "construct")
        function->returnType = function->ownerType->CreateReference();

    if (parser->currentToken.isNot(Token::Type::OpenCurly)) {
        parser->PrintSyntaxError("return type or '{'");
        return nullptr;
    }

    parser->NextToken(); // Consume '{'
    while (parser->currentToken.isNot(Token::Type::CloseCurly)) {
        auto expression = parseExpression(parser);
        if (expression == nullptr) return nullptr;

        function->body.push_back(expression);
    }

    if (parser->currentToken.isNot(Token::Type::CloseCurly)) {
        parser->PrintSyntaxError("'}'");
        return nullptr;
    }

    parser->NextToken(); // Consume '}'
    return function;
}
