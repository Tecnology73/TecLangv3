#include "StaticRef.h"
#include "../../compiler/Compiler.h"

TypeVariant* StaticRef::getFinalType() {
    auto type = Compiler::getScopeManager().getType(name);
    if (!type) return nullptr;

    if (next)
        return next->getFinalType(type->createVariant());

    return type->createVariant();
}

TypeVariant* StaticRef::getFinalType(TypeVariant* parentType) {
    auto type = parentType->type->GetField(name);
    if (!type) return nullptr;

    if (next)
        return next->getFinalType(type->type->ResolveType());

    return type->type->ResolveType();
}
