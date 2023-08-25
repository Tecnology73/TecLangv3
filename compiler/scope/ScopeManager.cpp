#include "ScopeManager.h"
#include <iostream>

ScopeManager *ScopeManager::instance = nullptr;

ScopeManager::ScopeManager(llvm::LLVMContext &context) : root(new RootScope("root")), current(root) {
    instance = this;

    addBuiltinType("bool", context);
    addBuiltinType("i8", context);
    addBuiltinType("i8*", context);
    addBuiltinType("i16", context);
    addBuiltinType("i32", context);
    addBuiltinType("int", context); // alias for i32
    addBuiltinType("i64", context);
    addBuiltinType("double", context);
    addBuiltinType("void", context);
}

ScopeManager *ScopeManager::get() {
    return instance;
}

void ScopeManager::leave(const std::string &name) {
    if (current->name != name) {
        std::cerr << "[Scope] Cannot leave scope '" << name << "' because current scope is '" << current->name << "'"
                  << std::endl;
        return;
    }

    if (current->parent != nullptr) {
        std::cout << "[Scope] Leave: " << current->parent->name << " < " << current->name << std::endl;
        current = current->parent;
    }
}

void ScopeManager::addBuiltinType(const std::string &name, llvm::LLVMContext &context) {
    auto type = new TypeDefinition(Token{
        .type = Token::Type::Identifier,
        .value = name
    }, name);

    if (type->name == "bool")
        type->setLlvmType(llvm::Type::getInt1Ty(context));
    else if (type->name == "i8")
        type->setLlvmType(llvm::Type::getInt8Ty(context));
    else if (type->name == "i8*")
        type->setLlvmType(llvm::Type::getInt8PtrTy(context));
    else if (type->name == "i16")
        type->setLlvmType(llvm::Type::getInt16Ty(context));
    else if (type->name == "i32" || type->name == "int")
        type->setLlvmType(llvm::Type::getInt32Ty(context));
    else if (type->name == "i64")
        type->setLlvmType(llvm::Type::getInt64Ty(context));
    else if (type->name == "double")
        type->setLlvmType(llvm::Type::getDoubleTy(context));
    else if (type->name == "void")
        type->setLlvmType(llvm::Type::getVoidTy(context));

    root->add(type);
    root->addCompiledType(type);
}

/*
 * AddField
 */

void ScopeManager::add(TypeDefinition *typeDef) const {
    root->add(typeDef);
}

void ScopeManager::addCompiledType(TypeDefinition *typeDef) const {
    root->addCompiledType(typeDef);
}

void ScopeManager::add(Enum *anEnum) const {
    root->add(anEnum);
}

void ScopeManager::add(Function *function) const {
    root->add(function);
}

void ScopeManager::add(VariableDeclaration *var) const {
    if (current == root) {
        // TODO: Use the ErrorManager
        std::cerr << "[Scope] Cannot AddField variable to root scope" << std::endl;
        return;
    }

    current->add(var);
}

/*
 * Has
 */

bool ScopeManager::hasType(const std::string &typeName) const {
    return root->hasType(typeName);
}

bool ScopeManager::hasEnum(const std::string &enumName) const {
    return root->hasEnum(enumName);
}

bool ScopeManager::hasFunction(const std::string &funcName) const {
    return root->hasFunction(funcName);
}

bool ScopeManager::hasVar(const std::string &varName) const {
    return current->hasVar(varName);
}

/*
 * Get
 */

TypeDefinition *ScopeManager::getType(const std::string &typeName) const {
    return root->getType(typeName);
}

TypeDefinition *ScopeManager::getType(llvm::Type *llvmType) const {
    return root->getType(llvmType);
}

Enum *ScopeManager::getEnum(const std::string &enumName) const {
    return root->getEnum(enumName);
}

Function *ScopeManager::getFunction(const std::string &funcName) const {
    return root->getFunction(funcName);
}

VariableDeclaration *ScopeManager::getVar(const std::string &varName) const {
    return current->getVar(varName);
}

/*
 * Codegen Context
 */

void ScopeManager::pushContext(CodegenContext *context) {
    if (!contextStack.empty())
        context->parent = contextStack.top();

    contextStack.push(context);
}

void ScopeManager::popContext() {
    delete contextStack.top();
    contextStack.pop();
}

bool ScopeManager::hasContext() const {
    return !contextStack.empty();
}

CodegenContext *ScopeManager::getContext() const {
    return contextStack.top();
}
