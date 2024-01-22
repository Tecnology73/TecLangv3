#include "TypeReference.h"
#include "../../compiler/Compiler.h"
#include "../../symbolTable/SymbolTable.h"

TypeVariant* TypeReference::ResolveType() {
    if (resolved)
        return variant;

    auto type = SymbolTable::GetInstance()->Get(name, SymbolType::Type | SymbolType::Enum);
    if (!type.has_value())
        return nullptr;

    if (type->type == SymbolType::Type)
        variant = std::get<TypeDefinition *>(type->value)->createVariant();
    else if (type->type == SymbolType::Enum)
        variant = std::get<Enum *>(type->value)->createVariant();

    resolved = true;
    return variant;
}
