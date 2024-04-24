#include "TypeDefinition.h"
#include "../../compiler/Compiler.h"
#include "../../symbolTable/SymbolTable.h"
#include "../StringInternTable.h"

TypeDefinition* TypeDefinition::Create(const Token& token) {
    auto& name = StringInternTable::Intern(token.value);
    auto type = SymbolTable::GetInstance()->Get<TypeDefinition>(name);
    if (!type) {
        type = new TypeDefinition(token, name);

        SymbolTable::GetInstance()->Add(type);
    }

    type->isDeclared = true;
    return type;
}

TypeBase* TypeDefinition::CreateUndeclared(const Token& token) {
    auto& name = StringInternTable::Intern(token.value);
    auto symbol = SymbolTable::GetInstance()->Get(name);
    if (symbol.has_value()) {
        if (symbol->type == SymbolType::Enum)
            return dynamic_cast<Enum*>(symbol->value);

        return dynamic_cast<TypeDefinition*>(symbol->value);
    }

    // Create a new type.
    auto type = new TypeDefinition(token, name);
    // Don't mark it as declared.

    SymbolTable::GetInstance()->Add(type);
    return type;
}
