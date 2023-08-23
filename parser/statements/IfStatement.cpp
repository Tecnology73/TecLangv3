#include "IfStatement.h"
#include "../expressions/Expression.h"

IfStatement *parseElseStatement(Parser *parser) {
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
        elseStatement = ifStatement;
    } else if (parser->currentToken.isNot(Token::Type::OpenCurly)) {
        parser->PrintSyntaxError("{");
        return nullptr;
    } else {
        parser->NextToken();
        while (parser->currentToken.isNot(Token::Type::CloseCurly)) {
            auto statement = parseExpression(parser);
            if (statement == nullptr)
                return nullptr;

            elseStatement->body.push_back(statement);
        }

        if (parser->currentToken.isNot(Token::Type::CloseCurly)) {
            parser->PrintSyntaxError("}");
            return nullptr;
        }

        parser->NextToken(); // Consume '}'
    }

    return elseStatement;
}

Node *parseIfCondition(Parser *parser) {
    /*auto hasOpenParen = parser->NextToken().is(Token::Type::OpenParen);
    if (hasOpenParen)
        parser->NextToken();*/

    auto condition = parseExpression(parser);
    if (condition == nullptr)
        return nullptr;

    /*if (hasOpenParen) {
        if (parser->currentToken.isNot(Token::Type::CloseParen)) {
            parser->PrintSyntaxError("Expected ')'");
            return nullptr;
        }

        parser->NextToken();
    }*/

    return condition;
}

IfStatement *parseIfStatement(Parser *parser) {
    if (parser->currentToken.isNot(Token::Type::If)) {
        parser->PrintSyntaxError("if'");
        return nullptr;
    }

    auto ifStatement = new IfStatement(parser->currentToken);
    parser->NextToken(); // Consume 'if'

    ifStatement->condition = parseIfCondition(parser);
    if (ifStatement->condition == nullptr)
        return nullptr;

    if (parser->currentToken.isNot(Token::Type::OpenCurly)) {
        parser->PrintSyntaxError("{");
        return nullptr;
    }

    parser->NextToken();
    while (parser->currentToken.isNot(Token::Type::CloseCurly)) {
        auto statement = parseExpression(parser);
        if (statement == nullptr)
            return nullptr;

        ifStatement->body.push_back(statement);
    }

    if (parser->currentToken.isNot(Token::Type::CloseCurly)) {
        parser->PrintSyntaxError("}");
        return nullptr;
    }

    if (parser->NextToken().is(Token::Type::Else)) {
        ifStatement->elseStatement = parseElseStatement(parser);
        if (ifStatement->elseStatement == nullptr)
            return nullptr;
    }

    return ifStatement;
}
