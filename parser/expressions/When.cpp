#include "When.h"
#include "Expression.h"
#include "../topLevel/Block.h"

bool parseWhenBlock(Parser *parser, WhenCondition *condition) {
    // When curly brace is omitted, it's a single expression.
    if (parser->currentToken.isNot(Token::Type::OpenCurly)) {
        auto expr = parseExpression(parser);
        if (!expr)
            return false;

        condition->body.push_back(expr);
        condition->implicitReturn = true;
        return true;
    }

    return parseBlock(parser, condition->body);
}

When *parseWhen(Parser *parser) {
    if (parser->currentToken.isNot(Token::Type::When)) {
        parser->PrintSyntaxError("when");
        return nullptr;
    }

    auto when = new When(parser->currentToken);
    auto hasOpenParen = parser->NextToken().is(Token::Type::OpenParen);
    if (hasOpenParen)
        parser->NextToken(); // Consumes '('

    // Condition
    when->condition = parseExpression(parser);
    if (hasOpenParen) {
        if (parser->currentToken.isNot(Token::Type::CloseParen)) {
            parser->PrintSyntaxError(")");
            return nullptr;
        }

        parser->NextToken(); // Consumes ')'
    }

    if (parser->currentToken.isNot(Token::Type::OpenCurly)) {
        parser->PrintSyntaxError("{");
        return nullptr;
    }

    // Body
    parser->NextToken(); // Consumes '{'
    while (parser->currentToken.isNot(Token::Type::CloseCurly)) {
        auto condition = new WhenCondition(parser->currentToken);
        if (parser->currentToken.isNot(Token::Type::Else)) {
            condition->condition = parseExpression(parser);
            if (!condition->condition)
                return nullptr;
        } else {
            parser->NextToken(); // Consumes 'else'

            if (when->hasElse) {
                parser->PrintSyntaxError("else");
                return nullptr;
            }

            when->hasElse = true;
        }

        if (parser->currentToken.isNot(Token::Type::Arrow)) {
            parser->PrintSyntaxError("->");
            return nullptr;
        }

        parser->NextToken(); // Consumes '->'
        if (!parseWhenBlock(parser, condition))
            return nullptr;

        when->body.push_back(condition);
    }

    if (parser->currentToken.isNot(Token::Type::CloseCurly)) {
        parser->PrintSyntaxError("}");
        return nullptr;
    }

    parser->NextToken(); // Consumes '}'
    return when;
}