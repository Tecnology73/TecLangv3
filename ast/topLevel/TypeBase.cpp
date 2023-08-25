#include "TypeBase.h"
#include "Function.h"
#include "../../compiler/Compiler.h"

llvm::Type *TypeBase::getLlvmType() {
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
        else
            isValueType = false;

        // Compiler::getScopeManager().addCompiledType(this);
    }

    if (!isValueType)
        return llvmType->getPointerTo();

    return llvmType;
}

llvm::Type *TypeBase::setLlvmType(llvm::Type *type) {
    llvmType = type;
    // Compiler::getScopeManager().addCompiledType(this);

    return type;
}

void TypeBase::AddField(TypeField *field) {
    fields[field->name] = field;
    field->index = fields.size() - 1;
    fieldOrder.push_back(field->name);
}

void TypeBase::AddFunction(Function *function) {
    if (functions.find(function->name) == functions.end())
        functions[function->name] = std::vector<Function *>();

    functions[function->name].push_back(function);
}

TypeField *TypeBase::GetField(const std::string &fieldName) const {
    auto it = fields.find(fieldName);
    if (it == fields.end())
        return nullptr;

    return it->second;
}

int TypeBase::GetFieldIndex(const std::string &fieldName) const {
    auto field = GetField(fieldName);
    if (!field)
        return -1;

    return field->index;
}

Function *TypeBase::GetFunction(const std::string &funcName) const {
    auto it = functions.find(funcName);
    if (it == functions.end())
        return nullptr;

    return it->second[0];
}

Function *TypeBase::FindFunction(const std::string &funcName, const std::vector<llvm::Value *> &parameters) const {
    if (functions.find(funcName) == functions.end())
        return nullptr;

    for (auto function: functions.at(funcName)) {
        if (function->parameters.size() != parameters.size()) continue;

        bool match = true;
        for (int i = 0; i < parameters.size(); i++) {
            auto type = Compiler::getScopeManager().getType(parameters[i]->getType());
            if (!type->canCastTo(function->GetParameter(i)->type)) {
                match = false;
                break;
            }
        }

        if (match) return function;
    }

    return nullptr;
}
