#include "FunctionCall.h"
#include "../../compiler/Compiler.h"

TypeVariant* FunctionCall::getFinalType() {
    auto function = Compiler::getScopeManager().getFunction(name);
    if (!function) return nullptr;

    if (next)
        return next->getFinalType(function->returnType->ResolveType());

    return function->returnType->ResolveType();
}

TypeVariant* FunctionCall::getFinalType(TypeVariant* parentType) {
    auto function = parentType->type->GetFunction(name);
    if (!function) return nullptr;

    if (next)
        return next->getFinalType(function->returnType->ResolveType());

    return function->returnType->ResolveType();
}

