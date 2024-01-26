#include "VarReference.h"
#include "../../compiler/Compiler.h"

TypeBase* VariableReference::getFinalType() {
    auto variable = Compiler::getScopeManager().GetVar(name);
    if (!variable) return nullptr;

    if (next)
        return next->getFinalType(variable->narrowedType->ResolveType());

    return variable->narrowedType->ResolveType();
}

TypeBase* VariableReference::getFinalType(const TypeBase* parentType) {
    auto type = parentType->GetField(name)->type;
    if (next)
        return next->getFinalType(type->ResolveType());

    return type->ResolveType();
}
