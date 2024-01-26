#include "StaticRef.h"
#include "../../compiler/Compiler.h"
#include "../../symbolTable/SymbolTable.h"

TypeBase* StaticRef::getFinalType() {
    auto type = SymbolTable::GetInstance()->Get(name);
    if (!type.has_value()) return nullptr;

    if (next)
        return next->getFinalType(type->value);

    return type->value;
}

TypeBase* StaticRef::getFinalType(const TypeBase* parentType) {
    auto type = parentType->GetField(name);
    if (!type) return nullptr;

    if (next)
        return next->getFinalType(type->type->ResolveType());

    return type->type->ResolveType();
}
