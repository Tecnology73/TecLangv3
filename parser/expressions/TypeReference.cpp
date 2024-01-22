#include "TypeReference.h"

TypeReference* parseTypeReference(Parser* parser) {
    if (parser->currentToken.isNot(Token::Type::Identifier)) {
        parser->PrintSyntaxError("type");
        return nullptr;
    }

    auto name = parser->currentToken;
    // TODO: Remove support for pointers.
    // This should only be available for the standard library.
    if (parser->NextToken().is(Token::Type::Multiply)) {
        // name.value += "*";
        parser->NextToken(); // Consume '*'
    }

    auto ref = new TypeReference(name, StringInternTable::Intern(name.value));
    if (parser->currentToken.is(Token::Type::Question)) {
        ref->flags.Set(TypeFlag::OPTIONAL);
        parser->NextToken(); // Consume '?'
    }

    return ref;
}
