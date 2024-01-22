#include "VarReference.h"
#include "../../compiler/Compiler.h"

TypeVariant* VariableReference::getFinalType() {
    auto variable = Compiler::getScopeManager().GetVar(name);
    if (!variable) return nullptr;

    if (next)
        return next->getFinalType(variable->narrowedType);

    return variable->narrowedType;
}

TypeVariant* VariableReference::getFinalType(TypeVariant* parentType) {
    auto type = parentType->type->GetField(name)->type;
    if (next)
        return next->getFinalType(type->ResolveType());

    return type->ResolveType();
}
