#include "StaticRef.h"
#include "VariableReference.h"

StaticRef *parseStaticRef(Parser *parser) {
    if (parser->currentToken.isNot(Token::Type::Identifier)) {
        parser->PrintSyntaxError("identifier");
        return nullptr;
    }

    auto ref = new StaticRef(parser->currentToken);
    ref->name = parser->currentToken.value;
    if (parser->NextToken().isNot(Token::Type::ColonColon)) {
        parser->PrintSyntaxError("::");
        return nullptr;
    }

    parser->NextToken(); // Consume '::'
    auto next = parseChainable(parser);
    if (!next) return nullptr;

    next->prev = ref;
    ref->next = next;

    return ref;
}