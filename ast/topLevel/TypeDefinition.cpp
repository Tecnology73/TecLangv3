#include "TypeDefinition.h"
#include "../../compiler/Compiler.h"
#include "../../compiler/TypeCoercion.h"
#include "../../symbolTable/SymbolTable.h"
#include "../StringInternTable.h"

llvm::Value* TypeDefinition::getDefaultValue() const {
    if (name == "i8" || name == "u8")
        return Compiler::getBuilder().getInt8(0);
    if (name == "i16" || name == "u16")
        return Compiler::getBuilder().getInt16(0);
    if (name == "i32" || name == "u32")
        return Compiler::getBuilder().getInt32(0);
    if (name == "i64" || name == "u64")
        return Compiler::getBuilder().getInt64(0);
    if (name == "double")
        return llvm::ConstantFP::get(Compiler::getBuilder().getDoubleTy(), 0);
    if (name == "bool")
        return Compiler::getBuilder().getInt1(false);
    if (name == "ptr")
        return llvm::ConstantPointerNull::get(Compiler::getBuilder().getPtrTy());

    if (isValueType)
        throw std::runtime_error("Cannot get default expression of non-type '" + name + "'!");

    if (flags.Has(TypeFlag::OPTIONAL))
        return llvm::ConstantPointerNull::get(llvmType->getPointerTo());

    // Create an instance of the type.
    auto ptr = Compiler::getBuilder().CreateCall(
        SymbolTable::GetInstance()->LookupFunction("malloc")[0]->llvmFunction,
        Compiler::getBuilder().getInt32(
            Compiler::getModule().getDataLayout().getTypeAllocSize(llvmType)
        )
    );

    Compiler::getBuilder().CreateCall(GetFunction("construct")->llvmFunction, {ptr});

    return ptr;
}

bool TypeDefinition::canCastTo(TypeBase* other) const {
    return TypeCoercion::canCoerceTo(this, other);
}

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
            return dynamic_cast<Enum *>(symbol->value);

        return dynamic_cast<TypeDefinition *>(symbol->value);
    }

    // Create a new type.
    auto type = new TypeDefinition(token, name);
    // Don't mark it as declared.

    SymbolTable::GetInstance()->Add(type);
    return type;
}
