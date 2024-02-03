#include "Enum.h"
#include "../../compiler/Compiler.h"
#include "../../symbolTable/SymbolTable.h"

Enum* Enum::Create(const Token& token, const std::string& name) {
    // FIXME: Don't overwrite a TypeDef if that already exists with the same name.
    if (auto existingEnum = SymbolTable::GetInstance()->Get<Enum>(name); existingEnum->isDeclared)
        return existingEnum;

    auto anEnum = new Enum(token, name);
    anEnum->isDeclared = true;

    SymbolTable::GetInstance()->Add(anEnum);

    return anEnum;
}
