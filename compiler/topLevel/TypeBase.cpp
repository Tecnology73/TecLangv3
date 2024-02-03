#include "TypeBase.h"
#include <llvm/IR/Constants.h>
#include "../Compiler.h"
#include "../../symbolTable/SymbolTable.h"

llvm::Value* getDefaultObjectValue(TypeReference* type) {
    if (type->name == "ptr" || type->flags.Has(TypeFlag::OPTIONAL))
        return llvm::ConstantPointerNull::get(Compiler::getBuilder().getPtrTy());

    // Create an instance of the type.
    // TODO: Use either alloca or malloc based on the lifetime of the object.
    auto ptr = Compiler::getBuilder().CreateCall(
        SymbolTable::GetInstance()->LookupFunction("malloc", {})->llvmFunction,
        Compiler::getBuilder().getInt32(
            Compiler::getModule().getDataLayout().getTypeAllocSize(type->ResolveType()->getLlvmType())
        )
    );

    Compiler::getBuilder().CreateCall(type->ResolveType()->GetFunction("construct")->llvmFunction, {ptr});

    return ptr;
}

llvm::Value* TypeCompiler::getDefaultValue(TypeReference* type) {
    if (!type->ResolveType()->isValueType)
        return getDefaultObjectValue(type);

    if (type->name == "i8" || type->name == "u8")
        return Compiler::getBuilder().getInt8(0);
    if (type->name == "i16" || type->name == "u16")
        return Compiler::getBuilder().getInt16(0);
    if (type->name == "i32" || type->name == "u32")
        return Compiler::getBuilder().getInt32(0);
    if (type->name == "i64" || type->name == "u64")
        return Compiler::getBuilder().getInt64(0);
    if (type->name == "double")
        return llvm::ConstantFP::get(Compiler::getBuilder().getDoubleTy(), 0);
    if (type->name == "bool")
        return Compiler::getBuilder().getInt1(false);

    throw std::runtime_error("Cannot get default expression of non-value type '" + type->name + "'!");
}

llvm::Type* compileValue(const TypeReference* type, TypeBase* concreteType) {
    if (type->name == "i8" || type->name == "u8")
        return concreteType->setLlvmType(Compiler::getBuilder().getInt8Ty());
    if (type->name == "i16" || type->name == "u16")
        return concreteType->setLlvmType(Compiler::getBuilder().getInt16Ty());
    if (type->name == "i32" || type->name == "u32")
        return concreteType->setLlvmType(Compiler::getBuilder().getInt32Ty());
    if (type->name == "i64" || type->name == "u64")
        return concreteType->setLlvmType(Compiler::getBuilder().getInt64Ty());
    if (type->name == "double")
        return concreteType->setLlvmType(Compiler::getBuilder().getDoubleTy());
    if (type->name == "bool")
        return concreteType->setLlvmType(Compiler::getBuilder().getInt1Ty());
    if (type->name == "void")
        return concreteType->setLlvmType(Compiler::getBuilder().getVoidTy());

    throw std::runtime_error("Cannot compile value type '" + type->name + "'!");
}

llvm::Type* compileObject(const TypeReference* type, TypeBase* concreteType) {
    if (type->name == "ptr")
        return concreteType->setLlvmType(Compiler::getBuilder().getPtrTy());

    std::vector<llvm::Type *> fields;
    for (const auto& fieldName: concreteType->fieldOrder) {
        auto field = concreteType->fields[fieldName];
        auto fieldConcreteType = field->type->ResolveType();
        TypeCompiler::compile(field->type);

        auto t = fieldConcreteType->llvmType;
        if (!fieldConcreteType->isValueType)
            t = t->getPointerTo();

        fields.push_back(t);
    }

    concreteType->setLlvmType(
        llvm::StructType::create(
            Compiler::getContext(),
            fields,
            type->name
        )
    );

    return concreteType->llvmType;
}

llvm::Type* TypeCompiler::compile(TypeReference* type) {
    auto concreteType = type->ResolveType();
    if (concreteType->llvmType)
        return concreteType->llvmType;

    if (concreteType->isValueType)
        return compileValue(type, concreteType);

    return compileObject(type, concreteType);
}
