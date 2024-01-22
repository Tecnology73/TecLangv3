#include "IfStatement.h"
#include "../expressions/Expression.h"

IfStatement* parseElseStatement(Parser* parser) {
    if (parser->currentToken.isNot(Token::Type::Else)) {
        parser->PrintSyntaxError("else");
        return nullptr;
    }

    auto elseStatement = new IfStatement(parser->currentToken);
    if (parser->NextToken().is(Token::Type::If)) {
        auto ifStatement = parseIfStatement(parser);
        if (ifStatement == nullptr)
            return nullptr;

        ifStatement->token = elseStatement->token;

        delete elseStatement;
        return elseStatement = ifStatement;
    }

    auto hasBlock = parser->currentToken.is(Token::Type::OpenCurly);
    if (hasBlock)
        parser->NextToken(); // Consume '{'

    while (parser->currentToken.isNot(Token::Type::CloseCurly)) {
        auto statement = parseExpression(parser);
        if (statement == nullptr)
            return nullptr;

        elseStatement->body.push_back(statement);

        if (!hasBlock)
            break;
    }

    if (hasBlock) {
        if (parser->currentToken.isNot(Token::Type::CloseCurly)) {
            parser->PrintSyntaxError("}");
            return nullptr;
        }

        parser->NextToken(); // Consume '}'
    }

    return elseStatement;
}

IfStatement* parseIfStatement(Parser* parser) {
    if (parser->currentToken.isNot(Token::Type::If)) {
        parser->PrintSyntaxError("if'");
        return nullptr;
    }

    auto ifStatement = new IfStatement(parser->currentToken);
    parser->NextToken(); // Consume 'if'

    ifStatement->condition = parseExpression(parser);
    if (ifStatement->condition == nullptr)
        return nullptr;

    auto hasBlock = parser->currentToken.is(Token::Type::OpenCurly);
    if (hasBlock)
        parser->NextToken(); // Consume '{'

    while (parser->currentToken.isNot(Token::Type::CloseCurly)) {
        auto statement = parseExpression(parser);
        if (statement == nullptr)
            return nullptr;

        ifStatement->body.push_back(statement);

        if (!hasBlock)
            break;
    }

    if (hasBlock) {
        if (parser->currentToken.isNot(Token::Type::CloseCurly)) {
            parser->PrintSyntaxError("}");
            return nullptr;
        }

        parser->NextToken(); // Consume '}'
    }

    if (parser->currentToken.is(Token::Type::Else)) {
        ifStatement->elseStatement = parseElseStatement(parser);
        if (ifStatement->elseStatement == nullptr)
            return nullptr;
    }

    return ifStatement;
}
