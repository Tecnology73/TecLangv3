#include "TypeDefinition.h"
#include "../../compiler/Compiler.h"
#include "../../compiler/TypeCoercion.h"
#include "../../symbolTable/SymbolTable.h"

llvm::Value* TypeDefinition::getDefaultValue() const {
    if (name == "i8")
        return Compiler::getBuilder().getInt8(0);
    if (name == "i16")
        return Compiler::getBuilder().getInt16(0);
    if (name == "i32" || name == "int")
        return Compiler::getBuilder().getInt32(0);
    if (name == "i64")
        return Compiler::getBuilder().getInt64(0);
    if (name == "double")
        return llvm::ConstantFP::get(Compiler::getBuilder().getDoubleTy(), 0);
    if (name == "bool")
        return Compiler::getBuilder().getInt1(false);

    if (isValueType)
        throw std::runtime_error("Cannot get default expression of non-type '" + name + "'!");

    if (flags.Has(TypeFlag::OPTIONAL))
        return llvm::ConstantPointerNull::get(llvmType->getPointerTo());

    // Create an instance of the type.
    auto ptr = Compiler::getBuilder().CreateCall(
        Compiler::getScopeManager().getFunction("malloc")->llvmFunction,
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

/*TypeVariant* TypeDefinition::Create(const Token& token) {
    return Create(token, token.value);
}

TypeVariant* TypeDefinition::Create(const Token& token, const std::string& name) {
    if (const auto existingType = Compiler::getScopeManager().getType(name, true))
        return existingType->createVariant();

    const auto type = new TypeDefinition(token, name);
    type->isDeclared = true;

    Compiler::getScopeManager().add(type);
    return type->createVariant();
}

TypeVariant* TypeDefinition::CreateTemporary(const Token& token) {
    if (const auto existingType = Compiler::getScopeManager().getType(token.value))
        return existingType->createVariant();

    const auto type = new TypeDefinition(token, token.value);
    Compiler::getScopeManager().add(type);
    return type->createVariant();
}*/

TypeDefinition* TypeDefinition::Create(const Token& token) {
    auto symbol = SymbolTable::GetInstance()->Get(StringInternTable::Intern(token.value));
    if (symbol.has_value()) {
        if (symbol->type != SymbolType::Type)
            return nullptr;

        auto type = std::get<TypeDefinition *>(symbol->value);
        type->isDeclared = true;

        return type;
    }

    auto type = new TypeDefinition(
        token,
        StringInternTable::Intern(token.value)
    );
    type->isDeclared = true;

    SymbolTable::GetInstance()->Add(type);

    return type;
}

TypeBase* TypeDefinition::CreateUndeclared(const Token& token) {
    auto symbol = SymbolTable::GetInstance()->Get(StringInternTable::Intern(token.value));
    if (symbol.has_value()) {
        if (symbol->type == SymbolType::Enum)
            return std::get<Enum *>(symbol->value);

        return std::get<TypeDefinition *>(symbol->value);
    }

    // Create a new type.
    auto type = new TypeDefinition(
        token,
        StringInternTable::Intern(token.value)
    );
    // Don't mark it as declared.

    SymbolTable::GetInstance()->Add(type);

    return type;
}
