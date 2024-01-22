#include "Identifier.h"
#include "VariableDeclaration.h"
#include "../expressions/VariableReference.h"
#include "../expressions/FunctionCall.h"
#include "../expressions/StaticRef.h"

Node* parseIdentifier(Parser* parser) {
    switch (parser->PeekToken().type) {
        // For assign (x = 1), we parse this as a BinaryOp and then promote
        // it to a VariableDeclaration during SemanticAnalysis if applicable.
        // case Token::Type::Assign:
        case Token::Type::Colon:
        // case Token::Type::Identifier:
        case Token::Type::Type:
            return parseVariableDeclaration(parser);
        case Token::Type::OpenParen:
            return parseFunctionCall(parser);
        case Token::Type::ColonColon:
            return parseStaticRef(parser);
        default:
            return parseVariableReference(parser);
    }
}
