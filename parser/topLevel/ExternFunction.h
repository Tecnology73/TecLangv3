#pragma once

#include "../Parser.h"
#include "../expressions/Function.h"

Function* parseExternFunction(Parser* parser) {
    if (parser->NextToken().isNot(Token::Type::Function)) {
        parser->PrintSyntaxError("func");
        return nullptr;
    }

    if (parser->PeekToken().isNot(Token::Type::Identifier)) {
        parser->NextToken(); // Consume 'func'
        parser->PrintSyntaxError("function name");
        return nullptr;
    }

    auto function = new Function(
        parser->currentToken,
        StringInternTable::Intern(parser->NextToken().value)
    );
    function->isExternal = true;

    SymbolTable::GetInstance()->Add(function);

    parser->NextToken(); // Consume '('
    if (!parseParameters(parser, function, true))
        return nullptr;

    if (parser->currentToken.isNot(Token::Type::Identifier)) {
        parser->PrintSyntaxError("return type");
        return nullptr;
    }

    function->returnType = parseTypeReference(parser);

    return function;
}
