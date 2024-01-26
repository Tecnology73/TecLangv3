#include "FunctionCall.h"
#include "../../compiler/Compiler.h"
#include "../../symbolTable/SymbolTable.h"

TypeBase* FunctionCall::getFinalType() {
    if (!function->returnType)
        return nullptr;

    if (next)
        return next->getFinalType(function->returnType->ResolveType());

    return function->returnType->ResolveType();
}

TypeBase* FunctionCall::getFinalType(const TypeBase* parentType) {
    auto function = parentType->GetFunction(name);
    if (!function) return nullptr;

    if (!function->returnType)
        return nullptr;

    if (next)
        return next->getFinalType(function->returnType->ResolveType());

    return function->returnType->ResolveType();
}

