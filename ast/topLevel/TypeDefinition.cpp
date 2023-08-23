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

    if (!isStruct)
        throw std::runtime_error("Cannot get default value of non-type '" + name + "'!");

    return Compiler::getBuilder().CreateCall(getFunction("construct")->llvmFunction);
}

bool TypeDefinition::canCastTo(TypeBase *other) const {
    return TypeCoercion::canCoerceTo((TypeBase *) this, other);
}

llvm::Type *TypeDefinition::setLlvmType(llvm::Type *type) {
    if (!llvmType) {
        llvmType = type;
        isStruct = llvm::isa<llvm::StructType>(type);
        // Register the compiled type for reverse lookups.
        Compiler::getScopeManager().addCompiledType(this);
    } else std::cout << "Type '" << name << "' has already been compiled!" << std::endl;

    return llvmType;
}

void TypeDefinition::addField(VariableDeclaration *field) {
    fields[field->name] = field;
    fieldIndices[field->name] = fields.size() - 1;
}

VariableDeclaration *TypeDefinition::getField(const std::string &fieldName) const {
    if (fields.find(fieldName) == fields.end())
        return nullptr;

    return fields.at(fieldName);
}

int TypeDefinition::getFieldIndex(const std::string &fieldName) {
    if (fieldIndices.find(fieldName) == fieldIndices.end())
        return -1;

    return fieldIndices[fieldName];
}

TypeDefinition *TypeDefinition::getFieldTypeDef(const std::string &fieldName) const {
    return dynamic_cast<TypeDefinition *>(fields.at(fieldName)->type);
}

void TypeDefinition::addFunction(Function *function) {
    if (functions.find(function->name) == functions.end())
        functions[function->name] = std::vector<Function *>();

    functions[function->name].push_back(function);
}

TypeDefinition *TypeDefinition::Create(const Token &token) {
    return Create(token, token.value);
}

TypeDefinition *TypeDefinition::Create(const Token &token, std::string name) {
    if (auto existingType = Compiler::getScopeManager().getType(name))
        return existingType;

    auto type = new TypeDefinition(token);
    type->name = name;

    Compiler::getScopeManager().add(type);
    return type;
}

/*
 * Function
 */

Function *TypeDefinition::getFunction(const std::string &name) const {
    if (functions.find(name) == functions.end())
        return nullptr;

    return functions.at(name)[0];
}

/**
 * Find a statement with the given funcName and parameters.
 *
 * @param funcName
 * @param parameters
 * @return
 */
Function *TypeDefinition::getFunction(const std::string &funcName, const std::vector<llvm::Value *> &parameters) const {
    if (functions.find(funcName) == functions.end())
        return nullptr;

    for (auto function: functions.at(funcName)) {
        if (function->parameters.size() != parameters.size()) continue;

        bool match = true;
        for (int i = 0; i < parameters.size(); i++) {
            auto type = Compiler::getScopeManager().getType(parameters[i]->getType());
            if (!type->canCastTo(function->parameters[i]->type)) {
                match = false;
                break;
            }
        }

        if (match) return function;
    }

    return nullptr;
}