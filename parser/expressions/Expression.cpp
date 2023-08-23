#include "Expression.h"
#include "../../ast/Expressions.h"
#include "../../ast/Literals.h"
#include "../literals/Integer.h"
#include "../literals/Double.h"
#include "../literals/Boolean.h"
#include "../literals/String.h"
#include "../statements/Return.h"
#include "../statements/Identifier.h"
#include "../statements/IfStatement.h"
#include "../statements/ForLoop.h"
#include "../statements/Continue.h"
#include "../statements/Break.h"
#include "Increment.h"
#include "ConstructorCall.h"
#include "When.h"

namespace {
    Node *parseFactor(Parser *parser) {
        switch (parser->currentToken.type) {
            case Token::Type::Return:
                return parseReturn(parser);
            case Token::Type::Integer:
                return parseInteger(parser);
            case Token::Type::Double:
                return parseDouble(parser);
            case Token::Type::String:
                return parseString(parser);
            case Token::Type::Identifier:
                return parseIdentifier(parser);
            case Token::Type::If:
                return parseIfStatement(parser);
            case Token::Type::For:
                return parseForLoop(parser);
            case Token::Type::Continue:
                return parseContinue(parser);
            case Token::Type::Break:
                return parseBreak(parser);
            case Token::Type::New:
                return parseConstructorCall(parser);
            case Token::Type::When:
                return parseWhen(parser);
            case Token::Type::Boolean_True:
            case Token::Type::Boolean_False:
                return parseBoolean(parser);
            case Token::Type::PlusPlus:
            case Token::Type::MinusMinus:
                return parseIncrement(parser);
            case Token::Type::OpenParen: {
                // Manual grouping of expression.
                parser->NextToken(); // Consume '('
                auto e = parseExpression(parser);
                if (e == nullptr)
                    return nullptr;

                if (parser->currentToken.isNot(Token::Type::CloseParen)) {
                    parser->PrintSyntaxError(")");
                    return nullptr;
                }

                parser->NextToken(); // Consume ')'
                return e;
            }
            default:
                parser->PrintSyntaxError("expression");
                return nullptr;
        }
    }

    // Order taken from here: https://en.cppreference.com/w/cpp/language/operator_precedence
    int getPrecedence(Token::Type type) {
        switch (type) {
            case Token::Type::Comma:
                return 0;
            case Token::Type::Assign:
            case Token::Type::PlusEqual:
            case Token::Type::MinusEqual:
            case Token::Type::MultiplyEqual:
            case Token::Type::DivideEqual:
            case Token::Type::PercentEqual:
            case Token::Type::AmpersandEqual:
            case Token::Type::CaretEqual:
            case Token::Type::PipeEqual:
                return 1;
            case Token::Type::Spread:
                return 2;
            case Token::Type::LogicalOr:
                return 3;
            case Token::Type::LogicalAnd:
                return 4;
            case Token::Type::Pipe:
                return 5;
            case Token::Type::Caret:
                return 6;
            case Token::Type::Ampersand:
                return 7;
            case Token::Type::Equal:
            case Token::Type::NotEqual:
                return 8;
            case Token::Type::LessThan:
            case Token::Type::GreaterThan:
            case Token::Type::LessThanOrEqual:
            case Token::Type::GreaterThanOrEqual:
                return 9;
            case Token::Type::Plus:
            case Token::Type::Minus:
                return 10;
            case Token::Type::Multiply:
            case Token::Type::Divide:
            case Token::Type::Percent:
                return 11;
            case Token::Type::Exclamation:
            case Token::Type::Tilde:
            case Token::Type::PlusPlus:
            case Token::Type::MinusMinus:
                return 12;
            default:
                return -1;
        }
    }
}

Node *parseExpression(Parser *parser, int minPrecedence) {
    Node *left = parseFactor(parser);
    if (left == nullptr)
        return nullptr;

    while (true) {
        Token op = parser->currentToken;
        int precedence = getPrecedence(op.type);
        if (precedence < minPrecedence)
            break;

        // edge-case because of how ++/-- are parsed.
        if (
            op.isNot(Token::Type::PlusPlus) &&
            op.isNot(Token::Type::MinusMinus)
            )
            parser->NextToken(); // Consume operator

        if (parser->currentToken.is(Token::Type::OpenParen)) {
            parser->NextToken(); // Consume '('
            Node *right = parseExpression(parser, 0);
            if (right == nullptr)
                return nullptr;

            left = BinaryOperation::Create(op, left, right);
            if (parser->currentToken.type != Token::Type::CloseParen) {
                parser->PrintSyntaxError(")");
                return nullptr;
            }

            parser->NextToken(); // Consume ')'
        } else {
            Node *right = parseExpression(parser, precedence + 1);
            if (right == nullptr)
                return nullptr;

            left = BinaryOperation::Create(op, left, right);
        }
    }

    /*if (parser->currentToken.is(Token::Type::Semicolon))
        parser->NextToken(); // Consume ';'*/

    return left;
}
