#include "TypeDefinition.h"
#include "../../compiler/Compiler.h"
#include "../../compiler/TypeCoercion.h"

llvm::Value *TypeDefinition::getDefaultValue() const {
    if (name == "i8")
        return Compiler::getBuilder().getInt8(0);
    else if (name == "i16")
        return Compiler::getBuilder().getInt16(0);
    else if (name == "i32" || name == "int")
        return Compiler::getBuilder().getInt32(0);
    else if (name == "i64")
        return Compiler::getBuilder().getInt64(0);
    else if (name == "double")
        return llvm::ConstantFP::get(Compiler::getBuilder().getDoubleTy(), 0);
    else if (name == "bool")
        return Compiler::getBuilder().getInt1(false);

    if (isValueType)
        throw std::runtime_error("Cannot get default expression of non-type '" + name + "'!");

    return Compiler::getBuilder().CreateCall(GetFunction("construct")->llvmFunction);
}

bool TypeDefinition::canCastTo(TypeBase *other) const {
    return TypeCoercion::canCoerceTo((TypeBase *) this, other);
}

TypeDefinition *TypeDefinition::Create(const Token &token) {
    return Create(token, token.value);
}

TypeDefinition *TypeDefinition::Create(const Token &token, std::string name) {
    if (auto existingType = Compiler::getScopeManager().getType(name))
        return existingType;

    auto type = new TypeDefinition(token, name);
    Compiler::getScopeManager().add(type);
    return type;
}
