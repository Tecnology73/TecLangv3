#include "TypeBase.h"
#include "TypeVariant.h"
#include "Function.h"
#include <llvm/IR/DerivedTypes.h>
#include "../../compiler/Compiler.h"
#include "../../compiler/statements/TypeDefinition.h"
#include "../../compiler/TypeCoercion.h"

TypeVariant* TypeBase::createVariant() {
    return new TypeVariant(this);
}

TypeReference* TypeBase::CreateReference() const {
    return new TypeReference(token, StringInternTable::Intern(name));
}

llvm::Type* TypeBase::getLlvmType() {
    if (!llvmType) {
        if (name == "bool")
            return setLlvmType(llvm::Type::getInt1Ty(Compiler::getContext()));
        else if (name == "i8")
            return setLlvmType(llvm::Type::getInt8Ty(Compiler::getContext()));
        else if (name == "i8*")
            return setLlvmType(llvm::Type::getInt8PtrTy(Compiler::getContext()));
        else if (name == "i16")
            return setLlvmType(llvm::Type::getInt16Ty(Compiler::getContext()));
        else if (name == "i32" || name == "int")
            return setLlvmType(llvm::Type::getInt32Ty(Compiler::getContext()));
        else if (name == "i64")
            return setLlvmType(llvm::Type::getInt64Ty(Compiler::getContext()));
        else if (name == "double")
            return setLlvmType(llvm::Type::getDoubleTy(Compiler::getContext()));
        else if (name == "void")
            return setLlvmType(llvm::Type::getVoidTy(Compiler::getContext()));
        else if (dynamic_cast<Enum *>(this))
            return setLlvmType(llvm::Type::getInt32Ty(Compiler::getContext()));

        // Compiler::getScopeManager().addCompiledType(this);
    }

    if (!isValueType)
        return llvmType->getPointerTo();

    return llvmType;
}

llvm::Type* TypeBase::setLlvmType(llvm::Type* type) {
    llvmType = type;
    // Compiler::getScopeManager().addCompiledType(this);

    return type;
}

void TypeBase::AddField(TypeField* field) {
    fields[field->name] = field;
    field->index = fields.size() - 1;
    fieldOrder.push_back(field->name);
}

void TypeBase::AddFunction(Function* function) {
    auto name = std::string(function->name);
    if (functions.find(name) == functions.end())
        functions[name] = std::vector<Function *>();

    functions[name].push_back(function);
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
    const std::vector<llvm::Value *>& parameters,
    const std::vector<const TypeVariant *>& paramTypes
) const {
    if (functions.find(funcName) == functions.end())
        return nullptr;

    for (auto function: functions.at(funcName)) {
        if (function->parameters.size() != parameters.size()) continue;

        bool match = true;
        for (int i = 0; i < parameters.size(); i++) {
            auto arg = function->GetParameter(i);
            auto variant = paramTypes[i];

            if (!TypeCoercion::isTypeCompatible(variant->type, arg->type->ResolveType()->type)) {
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
