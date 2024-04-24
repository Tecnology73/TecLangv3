#include "TypeReference.h"
#include "../StringInternTable.h"
#include "../../symbolTable/SymbolTable.h"

TypeReference::TypeReference(const Token& token, const std::string& name) : Node(token), name(name) {
}

TypeReference::TypeReference(TypeBase* type) : Node(type->token), name(StringInternTable::Intern(type->name)) {
    concreteType = type;
    resolved = true;
}

void TypeReference::Accept(Visitor* visitor) {
    visitor->Visit(this);
}

TypeReference* TypeReference::Clone() const {
    auto clone = new TypeReference(token, name);
    clone->flags = flags;

    // If we've already resolved the concreteType on this reference,
    // we might as well copy that over so the clone doesn't have to do another resolve.
    if (concreteType) {
        clone->concreteType = concreteType;
        clone->resolved = true;
    }

    return clone;
}

TypeBase* TypeReference::ResolveType() {
    if (resolved)
        return concreteType;

    // Regardless of whether the type is found or not, we want to mark it as resolved.
    resolved = true;

    const auto type = SymbolTable::GetInstance()->Get(name);
    if (!type.has_value())
        return nullptr;

    return concreteType = type->value;
}
