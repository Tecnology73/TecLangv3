#include "VariableReference.h"
#include "FunctionCall.h"
#include "StaticRef.h"

ChainableNode* parseChainable(Parser* parser) {
    switch (parser->PeekToken().type) {
        case Token::Type::OpenParen:
            return parseFunctionCall(parser);
        case Token::Type::ColonColon:
            return parseStaticRef(parser);
        default:
            return parseVariableReference(parser);
    }
}

VariableReference* parseVariableReference(Parser* parser) {
    if (parser->currentToken.isNot(Token::Type::Identifier, Token::Type::Type)) {
        parser->PrintSyntaxError("identifier");
        return nullptr;
    }

    auto node = new VariableReference(
        parser->currentToken,
        StringInternTable::Intern(parser->currentToken.value)
    );

    // Parse the next varRef/funcCall (e.g. this.x or this.x());
    if (parser->NextToken().is(Token::Type::Period)) {
        parser->NextToken(); // Consume '.'
        auto next = parseChainable(parser);
        if (!next) return nullptr;

        next->prev = node;
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
