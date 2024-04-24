#include "TypeBase.h"
#include "Function.h"
#include <llvm/IR/DerivedTypes.h>
#include "../../compiler/Compiler.h"
#include "../../compiler/TypeCoercion.h"

TypeReference* TypeBase::CreateReference() {
    return new TypeReference(this);
}

llvm::Type* TypeBase::getLlvmType() {
    if (!llvmType) {
        if (name == "i8" || name == "u8")
            return setLlvmType(Compiler::getBuilder().getInt8Ty());
        if (name == "i16" || name == "u16")
            return setLlvmType(Compiler::getBuilder().getInt16Ty());
        if (name == "i32" || name == "u32")
            return setLlvmType(Compiler::getBuilder().getInt32Ty());
        if (name == "i64" || name == "u64")
            return setLlvmType(Compiler::getBuilder().getInt64Ty());
        if (name == "double")
            return setLlvmType(Compiler::getBuilder().getDoubleTy());
        if (name == "bool")
            return setLlvmType(Compiler::getBuilder().getInt1Ty());
        if (name == "void")
            return setLlvmType(Compiler::getBuilder().getVoidTy());
        if (name == "ptr")
            return setLlvmType(Compiler::getBuilder().getPtrTy());
    }

    if (!isValueType)
        return llvmType->getPointerTo();

    return llvmType;
}

llvm::Type* TypeBase::setLlvmType(llvm::Type* type) {
    llvmType = type;

    return type;
}

void TypeBase::AddField(TypeField* field) {
    fields[field->name] = field;
    field->index = fields.size() - 1;
    fieldOrder.push_back(field->name);
}

void TypeBase::AddFunction(Function* function) {
    if (!functions.contains(function->name))
        functions[function->name] = std::vector<Function*>();

    functions[function->name].push_back(function);
}

TypeField* TypeBase::GetField(const std::string& fieldName) const {
    auto it = fields.find(fieldName);
    if (it == fields.end())
        return nullptr;

    return it->second;
}

int TypeBase::GetFieldIndex(const std::string& fieldName) const {
    auto field = GetField(fieldName);
    if (!field)
        return -1;

    return field->index;
}

Function* TypeBase::GetFunction(const std::string& funcName) const {
    auto it = functions.find(funcName);
    if (it == functions.end())
        return nullptr;

    return it->second[0];
}

Function* TypeBase::FindFunction(
    const std::string& funcName,
    const std::vector<TypeReference*>& paramTypes
) const {
    auto it = functions.find(funcName);
    if (it == functions.end())
        return nullptr;

    for (const auto& function: it->second) {
        if (function->parameters.size() != paramTypes.size()) continue;

        bool match = true;
        for (int i = 0; i < paramTypes.size(); i++) {
            auto arg = function->GetParameter(i);
            auto paramType = paramTypes[i];

            if (!TypeCoercion::isTypeCompatible(paramType, arg->type)) {
                match = false;
                break;
            }
        }

        if (match) return function;
    }

    return nullptr;
}

void TypeField::Accept(Visitor* visitor) {
    visitor->Visit(this);
}
