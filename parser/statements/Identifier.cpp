#include "Identifier.h"
#include "VariableDeclaration.h"
#include "../expressions/VariableReference.h"
#include "../expressions/FunctionCall.h"
#include "../expressions/StaticRef.h"

Node *parseIdentifier(Parser *parser) {
    switch (parser->PeekToken().type) {
        case Token::Type::Assign:
        case Token::Type::Colon:
        case Token::Type::Identifier:
            return parseVariableDeclaration(parser);
        case Token::Type::OpenParen:
            return parseFunctionCall(parser);
        case Token::Type::ColonColon:
            return parseStaticRef(parser);
        default:
            return parseVariableReference(parser);
    }
}
