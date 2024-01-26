#include "Enum.h"
#include "../../compiler/Compiler.h"
#include "../../symbolTable/SymbolTable.h"

llvm::Value* Enum::getDefaultValue() const {
    return Compiler::getBuilder().getInt32(0);
}

bool Enum::canCastTo(TypeBase* other) const {
    return false;
}

Enum* Enum::Create(const Token& token, const std::string& name) {
    // FIXME: Don't overwrite a TypeDef if that already exists with the same name.
    if (auto existingEnum = SymbolTable::GetInstance()->Get<Enum>(name); existingEnum->isDeclared)
        return existingEnum;

    auto anEnum = new Enum(token, name);
    anEnum->isDeclared = true;

    SymbolTable::GetInstance()->Add(anEnum);

    return anEnum;
}
