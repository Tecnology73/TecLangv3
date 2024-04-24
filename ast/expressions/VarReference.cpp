#include "VarReference.h"
#include "../../scope/Scope.h"
#include "../expressions/TypeReference.h"

TypeBase* VariableReference::getFinalType() {
    auto variable = Scope::GetScope()->Get(name);
    if (!variable) return nullptr;

    if (next)
        return next->getFinalType(variable->type->ResolveType());

    return variable->type->ResolveType();
}

TypeBase* VariableReference::getFinalType(const TypeBase* parentType) {
    auto type = parentType->GetField(name)->type;
    if (next)
        return next->getFinalType(type->ResolveType());

    return type->ResolveType();
}
