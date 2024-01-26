#include "StaticRef.h"
#include "VariableReference.h"
#include "../../ast/StringInternTable.h"

StaticRef* parseStaticRef(Parser* parser) {
    if (parser->currentToken.isNot(Token::Type::Identifier)) {
        parser->PrintSyntaxError("identifier");
        return nullptr;
    }

    auto ref = new StaticRef(
        parser->currentToken,
        StringInternTable::Intern(parser->currentToken.value)
    );
    if (parser->NextToken().is(Token::Type::ColonColon)) {
        parser->NextToken(); // Consume '::'

        auto next = parseChainable(parser);
        if (!next) return nullptr;

        next->prev = ref;
        ref->next = next;
    }

    return ref;
}
