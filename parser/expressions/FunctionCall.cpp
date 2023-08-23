#include "FunctionCall.h"
#include "Expression.h"

FunctionCall *parseFunctionCall(Parser *parser) {
    if (parser->currentToken.isNot(Token::Type::Identifier)) {
        parser->PrintSyntaxError("identifier");
        return nullptr;
    }

    auto node = new FunctionCall(parser->currentToken);
    node->name = parser->currentToken.value;

    if (parser->NextToken().isNot(Token::Type::OpenParen)) {
        parser->PrintSyntaxError("(");
        return nullptr;
    }

    // Parse parameters.
    parser->NextToken(); // Consume '('
    while (parser->currentToken.isNot(Token::Type::CloseParen)) {
        // `minPrecedence = 1` - So we don't get a BinaryOp(Comma, ...)
        auto parameter = parseExpression(parser, 1);
        if (parameter == nullptr)
            return nullptr;

        node->arguments.push_back(parameter);

        if (parser->currentToken.is(Token::Type::Comma)) {
            parser->NextToken(); // Consume ','
        } else if (parser->currentToken.isNot(Token::Type::CloseParen)) {
            parser->PrintSyntaxError(",  or )");
            return nullptr;
        }
    }

    if (parser->currentToken.isNot(Token::Type::CloseParen)) {
        parser->PrintSyntaxError(")");
        return nullptr;
    }

    // Parse the next varRef/funcCall (e.g. this.x or this.x());
    if (parser->NextToken().is(Token::Type::Period)) {
        parser->NextToken(); // Consume '.'
        auto next = parseExpression(parser);
        if (!next) return nullptr;
        if (auto varRef = dynamic_cast<VariableReference *>(next))
            node->next = varRef;
        else if (auto funcCall = dynamic_cast<FunctionCall *>(next))
            node->next = funcCall;
        else {
            parser->PrintSyntaxError("variable reference or statement call");
            return nullptr;
        }
    }

    return node;
}