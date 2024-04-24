#include "Symbol.h"
#include "../ast/statements/VarDeclaration.h"

Symbol::Symbol(Node* node, TypeReference* typeRef) : node(node), type(typeRef->Clone()) {
}

Symbol::~Symbol() {
    delete type;
}

std::shared_ptr<Symbol> Symbol::GetField(const std::string& name) {
    if (auto it = fieldCache.find(name); it != fieldCache.end())
        return it->second;

    // Clone the source type and store it on here so that it can be modified as we see fit.
    if (auto field = type->ResolveType()->GetField(name))
        return fieldCache[name] = std::make_shared<Symbol>(field, field->type);

    return nullptr;
}
