#include "TypeDefinition.h"
#include "VariableDeclaration.h"

TypeDefinition *parseTypeDefinition(Parser *parser) {
    if (parser->currentToken.isNot(Token::Type::Type)) {
        parser->PrintSyntaxError("type");
        return nullptr;
    }

    if (parser->PeekToken().isNot(Token::Type::Identifier)) {
        parser->PrintSyntaxError("type name");
        return nullptr;
    }

    /*auto typeDef = TypeDefinition::Create(parser->NextToken());
    if (parser->NextToken().isNot(Token::Type::OpenCurly)) {
        parser->PrintSyntaxError("{");
        return nullptr;
    }

    parser->NextToken(); // Consume '{'
    while (parser->currentToken.isNot(Token::Type::CloseCurly)) {
        auto node = parseVariableDeclaration(parser);
        if (node == nullptr) return nullptr;

        typeDef->add(node);
    }

    if (parser->currentToken.isNot(Token::Type::CloseCurly)) {
        parser->PrintSyntaxError("}");
        return nullptr;
    }

    parser->NextToken(); // Consume '}'
    return typeDef;*/
    return nullptr;
}