#include "SymbolTable.h"
#include "statements/TypeDefinition.h"

/*
 * Scope Node
 */

bool ScopeNode::addVariable(VariableDeclaration *node) {
    if (variables.find(node->name) != variables.end()) {
        std::cout << "Variable '" << node->name << "' has already been declared." << std::endl;
        return false;
    }

    variables[node->name] = node;
    return true;
}

bool ScopeNode::addFunction(Function *node) {
    if (functions.find(node->name) != functions.end()) {
        std::cout << "Function '" << node->name << "' has already been declared." << std::endl;
        return false;
    }

    functions[node->name] = node;
    return true;
}

bool ScopeNode::addType(TypeDefinition *node) {
    if (types.find(node->name) != types.end()) {
        std::cout << "Type '" << node->name << "' has already been declared." << std::endl;
        return false;
    }

    types[node->name] = node;
    return true;
}

bool ScopeNode::addEnum(Enum *node) {
    if (enums.find(node->name) != enums.end()) {
        std::cout << "Enum '" << node->name << "' has already been declared." << std::endl;
        return false;
    }

    enums[node->name] = node;
    return true;
}

bool ScopeNode::hasVariable(const std::string &varName) const {
    auto it = variables.find(varName);
    if (it != variables.end())
        return true;

    if (parent)
        return parent->hasVariable(varName);

    return false;
}

bool ScopeNode::hasFunction(const std::string &funcName) const {
    auto it = functions.find(funcName);
    if (it != functions.end())
        return true;

    if (parent)
        return parent->hasFunction(funcName);

    return false;
}

bool ScopeNode::hasType(const std::string &typeName) const {
    auto it = types.find(typeName);
    if (it != types.end())
        return true;

    if (parent)
        return parent->hasType(typeName);

    return false;
}

bool ScopeNode::hasEnum(const std::string &typeName) const {
    auto it = enums.find(typeName);
    if (it != enums.end())
        return true;

    if (parent)
        return parent->hasType(typeName);

    return false;
}

VariableDeclaration *ScopeNode::lookupVariable(const std::string &varName) {
    auto it = variables.find(varName);
    if (it != variables.end()) {
        return it->second;
    }

    if (parent != nullptr) {
        return parent->lookupVariable(varName);
    }

    return nullptr;
}

Function *ScopeNode::lookupFunction(const std::string &name) {
    if (functions.find(name) != functions.end()) {
        return functions[name];
    }

    if (parent != nullptr) {
        return parent->lookupFunction(name);
    }

    return nullptr;
}

TypeDefinition *ScopeNode::lookupType(const std::string &typeName) {
    auto it = types.find(typeName);
    if (it != types.end())
        return it->second;

    if (parent != nullptr) {
        return parent->lookupType(typeName);
    }

    return nullptr;
}

TypeDefinition *ScopeNode::lookupType(llvm::Type *type) const {
    auto it = compiledTypes.find(type);
    if (it != compiledTypes.end())
        return it->second;

    if (parent != nullptr)
        return parent->lookupType(type);

    return nullptr;
}

Enum *ScopeNode::lookupEnum(const std::string &enumName) const {
    auto it = enums.find(enumName);
    if (it != enums.end())
        return it->second;

    if (parent != nullptr)
        return parent->lookupEnum(enumName);

    return nullptr;
}

void ScopeNode::setLlvmType(TypeDefinition *type) {
    if (compiledTypes.find(type->llvmType) != compiledTypes.end()) {
        std::cout << "Type '" << type->name << "' has already been registered!" << std::endl;
        return;
    }

    compiledTypes[type->llvmType] = type;
}

Function *ScopeNode::getCurrentFunction() const {
    if (function)
        return function;

    if (parent)
        return parent->getCurrentFunction();

    return nullptr;
}

VariableDeclaration *ScopeNode::getCurrentVariable() const {
    if (variable)
        return variable;

    if (parent)
        return parent->getCurrentVariable();

    return nullptr;
}

When *ScopeNode::getCurrentWhen() const {
    if (when)
        return when;

    if (parent)
        return parent->getCurrentWhen();

    return nullptr;
}

/*
 * Symbol Table
 */

SymbolTable::SymbolTable(llvm::LLVMContext &context) : rootScope(new ScopeNode("Root")), currentScopeNode(rootScope) {
    addBuiltinType("bool", context);
    addBuiltinType("i8", context);
    addBuiltinType("i16", context);
    addBuiltinType("i32", context);
    addBuiltinType("i64", context);
    addBuiltinType("double", context);
}

void SymbolTable::enterScope(std::string name) {
    std::cout << "[Scope] " << currentScopeNode->name
              << " > " << name
              << std::endl;

    auto childNode = new ScopeNode(std::move(name));
    childNode->parent = currentScopeNode;
    currentScopeNode = childNode;
}

void SymbolTable::enterScope(std::string name, Function *function) {
    std::cout << "[Scope] " << currentScopeNode->name
              << " > " << name
              << std::endl;

    auto childNode = new ScopeNode(std::move(name), function);
    childNode->parent = currentScopeNode;
    currentScopeNode = childNode;
}

void SymbolTable::enterScope(std::string name, VariableDeclaration *var) {
    std::cout << "[Scope] " << currentScopeNode->name
              << " > " << name
              << std::endl;

    auto childNode = new ScopeNode(std::move(name), var);
    childNode->parent = currentScopeNode;
    currentScopeNode = childNode;
}

void SymbolTable::enterScope(std::string name, When *when) {
    std::cout << "[Scope] " << currentScopeNode->name
              << " > " << name
              << std::endl;

    auto childNode = new ScopeNode(std::move(name), when);
    childNode->parent = currentScopeNode;
    currentScopeNode = childNode;
}

void SymbolTable::exitScope() {
    if (currentScopeNode != rootScope) {
        std::cout << "[Scope] " << currentScopeNode->parent->name
                  << " < " << currentScopeNode->name
                  << std::endl;

        ScopeNode *parent = currentScopeNode->parent;
        // Uncomment this and watch shit break...
        // delete currentScopeNode;
        currentScopeNode = parent;
    } else {
        std::cout << "Cannot exit root scope." << std::endl;
    }
}

bool SymbolTable::addVariable(VariableDeclaration *node) {
    return currentScopeNode->addVariable(node);
}

bool SymbolTable::addFunction(Function *node) {
    return currentScopeNode->addFunction(node);
}

bool SymbolTable::addType(TypeDefinition *node) {
    return rootScope->addType(node);
}

bool SymbolTable::hasVariable(const std::string &varName) const {
    return currentScopeNode->hasVariable(varName);
}

bool SymbolTable::hasFunction(const std::string &funcName) const {
    return currentScopeNode->hasFunction(funcName);
}

bool SymbolTable::hasType(const std::string &typeName) const {
    return currentScopeNode->hasType(typeName);
}

VariableDeclaration *SymbolTable::lookupVariable(const std::string &name) const {
    return currentScopeNode->lookupVariable(name);
}

Function *SymbolTable::lookupFunction(const std::string &name) const {
    return currentScopeNode->lookupFunction(name);
}

TypeDefinition *SymbolTable::lookupType(const std::string &name) const {
    return rootScope->lookupType(name);
}

TypeDefinition *SymbolTable::lookupType(llvm::Type *type) const {
    return rootScope->lookupType(type);
}

Enum *SymbolTable::lookupEnum(const std::string &name) const {
    return rootScope->lookupEnum(name);
}

Function *SymbolTable::getCurrentFunction() const {
    return currentScopeNode->getCurrentFunction();
}

VariableDeclaration *SymbolTable::getCurrentVariable() const {
    return currentScopeNode->getCurrentVariable();
}

When *SymbolTable::getCurrentWhen() const {
    return currentScopeNode->getCurrentWhen();
}

ScopeNode *SymbolTable::getRootScope() {
    return rootScope;
}

void SymbolTable::addBuiltinType(const std::string &name, llvm::LLVMContext &context) {
    auto type = new TypeDefinition(Token{
        .type = Token::Type::Identifier,
        .value = name
    });
    type->name = name;

    rootScope->addType(type);

    if (type->name == "bool")
        type->llvmType = llvm::Type::getInt1Ty(context);
    else if (type->name == "i8")
        type->llvmType = llvm::Type::getInt8Ty(context);
    else if (type->name == "i8*")
        type->llvmType = llvm::Type::getInt8PtrTy(context);
    else if (type->name == "i16")
        type->llvmType = llvm::Type::getInt16Ty(context);
    else if (type->name == "i32" || type->name == "int")
        type->llvmType = llvm::Type::getInt32Ty(context);
    else if (type->name == "i64")
        type->llvmType = llvm::Type::getInt64Ty(context);
    else if (type->name == "double")
        type->llvmType = llvm::Type::getDoubleTy(context);
    else if (type->name == "void")
        type->llvmType = llvm::Type::getVoidTy(context);

    rootScope->setLlvmType(type);
}
