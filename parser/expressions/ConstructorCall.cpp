#include "ConstructorCall.h"
#include "Expression.h"
#include "../../ast/Statements.h"

bool parseArguments(Parser *parser, ConstructorCall *c) {
    if (parser->currentToken.isNot(Token::Type::OpenParen)) {
        parser->PrintSyntaxError("(");
        return false;
    }

    parser->NextToken(); // Consume '('
    while (parser->currentToken.isNot(Token::Type::CloseParen)) {
        auto arg = parseExpression(parser, 1);
        if (!arg)
            return false;

        c->arguments.push_back(arg);

        if (parser->currentToken.is(Token::Type::CloseParen))
            break;

        if (parser->currentToken.isNot(Token::Type::Comma)) {
            parser->PrintSyntaxError(", or )");
            return false;
        }

        parser->NextToken(); // Consume ','
    }

    if (parser->currentToken.isNot(Token::Type::CloseParen)) {
        parser->PrintSyntaxError(")");
        return false;
    }

    parser->NextToken(); // Consume ')'
    return true;
}

bool parseAssignments(Parser *parser, ConstructorCall *c) {
    if (parser->currentToken.isNot(Token::Type::OpenCurly)) {
        parser->PrintSyntaxError("{");
        return false;
    }

    parser->NextToken(); // Consume '{'
    while (parser->currentToken.isNot(Token::Type::CloseCurly)) {
        if (parser->currentToken.isNot(Token::Type::Identifier)) {
            parser->PrintSyntaxError("identifier");
            return false;
        }

        auto var = parseExpression(parser, 1);
        if (auto varDecl = dynamic_cast<VariableDeclaration *>(var))
            c->assignments.push_back(varDecl);
        else
            return false;

        if (parser->currentToken.is(Token::Type::CloseCurly))
            break;

        if (parser->currentToken.isNot(Token::Type::Comma)) {
            parser->PrintSyntaxError(", or }");
            return false;
        }

        parser->NextToken(); // Consume ','
    }

    if (parser->currentToken.isNot(Token::Type::CloseCurly)) {
        parser->PrintSyntaxError("}");
        return false;
    }

    parser->NextToken(); // Consume '}'
    return true;
}

ConstructorCall *parseConstructorCall(Parser *parser) {
    if (parser->currentToken.isNot(Token::Type::New)) {
        parser->PrintSyntaxError("new");
        return nullptr;
    }

    if (parser->PeekToken().isNot(Token::Type::Identifier)) {
        parser->PrintSyntaxError("identifier");
        return nullptr;
    }

    auto node = new ConstructorCall(parser->currentToken);
    if (parser->NextToken().isNot(Token::Type::Identifier)) {
        parser->PrintSyntaxError("identifier");
        return nullptr;
    }

    node->type = TypeDefinition::Create(parser->currentToken, parser->currentToken.value);
    parser->NextToken(); // Consume identifier

    if (parser->currentToken.is(Token::Type::OpenParen)) {
        if (!parseArguments(parser, node))
            return nullptr;
    } else if (parser->currentToken.is(Token::Type::OpenCurly)) {
        if (!parseAssignments(parser, node))
            return nullptr;
    } else {
        parser->PrintSyntaxError("( or {");
        return nullptr;
    }

    return node;
}