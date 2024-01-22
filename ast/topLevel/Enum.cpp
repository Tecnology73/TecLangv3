#include "Enum.h"
#include "../../compiler/Compiler.h"

llvm::Value* Enum::getDefaultValue() const {
    return Compiler::getBuilder().getInt32(0);
}

bool Enum::canCastTo(TypeBase* other) const {
    return false;
}

Enum* Enum::Create(const Token& token, const std::string& name) {
    if (auto existingEnum = Compiler::getScopeManager().getEnum(name, true))
        return existingEnum;

    auto anEnum = new Enum(token, name);
    anEnum->isDeclared = true;

    Compiler::getScopeManager().add(anEnum);

    return anEnum;
}
