#include "ScopeManager.h"
#include <llvm/IR/DerivedTypes.h>

ScopeManager* ScopeManager::instance = nullptr;

ScopeManager::ScopeManager(llvm::LLVMContext& context) : root(new RootScope("root")), current(root) {
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
    addBuiltinType("null", context);
}

ScopeManager* ScopeManager::get() {
    return instance;
}

void ScopeManager::leave(const std::string& name) {
    if (current->name != name) {
        printError("Cannot leave scope '%s' because current scope is '%s'", name.c_str(), current->name.c_str());
        return;
    }

    if (current->parent != nullptr) {
        // printDebug("[Scope] Leave: %s < %s", current->parent->name.c_str(), current->name.c_str());
        current = current->parent;
    }
}

Scope* ScopeManager::GetParentScope(unsigned levels) const {
    auto scope = current;
    while (levels-- > 0 && scope->parent != nullptr)
        scope = scope->parent;

    return scope;
}

void ScopeManager::addBuiltinType(const std::string& name, llvm::LLVMContext& context) {
    auto type = new TypeDefinition(
        Token{
            .type = Token::Type::Identifier,
            .value = name
        },
        name
    );
    type->isDeclared = true;
    type->isValueType = true;

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
 * Add
 */

void ScopeManager::add(TypeDefinition* typeDef) const {
    root->add(typeDef);
}

void ScopeManager::addCompiledType(TypeDefinition* typeDef) const {
    root->addCompiledType(typeDef);
}

void ScopeManager::add(Enum* anEnum) const {
    if (auto type = root->getType(anEnum->name)) {
        if (type->isDeclared) {
            printError("'%s' has already been declared!", anEnum->name.c_str());
            exit(1);
        }
    }

    root->add(anEnum);
}

void ScopeManager::add(Function* function) const {
    root->add(function);
}

/*void ScopeManager::add(VariableDeclaration* var) const {
    if (current == root) {
        // TODO: Use the ErrorManager
        printError("[Scope] Cannot add variable to root scope!");
        return;
    }

    current->add(var);
}*/

void ScopeManager::Add(const VariableDeclaration* var) const {
    if (current == root) {
        // TODO: Use the ErrorManager
        printError("[Scope] Cannot add variable to root scope!");
        return;
    }

    current->Add(var);
}

void ScopeManager::Add(const std::string& name, const Node* node, const TypeVariant* narrowedType) const {
    if (current == root) {
        // TODO: Use the ErrorManager
        printError("[Scope] Cannot add variable to root scope!");
        return;
    }

    current->Add(name, node, narrowedType);
}

void ScopeManager::addTypeUse(TypeVariant* variant, const TypeBase* type) const {
    root->addTypeUse(variant, type);
}

/*
 * Has
 */

bool ScopeManager::hasType(const std::string& typeName) const {
    return root->hasType(typeName);
}

bool ScopeManager::hasEnum(const std::string& enumName) const {
    return root->hasEnum(enumName);
}

bool ScopeManager::hasFunction(const std::string& funcName) const {
    return root->hasFunction(funcName);
}

/*bool ScopeManager::hasVar(const std::string& varName) const {
    return current->hasVar(varName);
}*/

bool ScopeManager::HasVar(const std::string& name) const {
    return current->HasVar(name);
}


/*
 * Get
 */

TypeBase* ScopeManager::getType(const std::string& typeName, bool onlyDeclared) const {
    return root->getType(typeName, onlyDeclared);
}

TypeDefinition* ScopeManager::getType(const llvm::Type* llvmType) const {
    return root->getType(llvmType);
}

Enum* ScopeManager::getEnum(const std::string& enumName, bool onlyDeclared) const {
    return root->getEnum(enumName, onlyDeclared);
}

Function* ScopeManager::getFunction(const std::string& funcName) const {
    return root->getFunction(funcName);
}

/*VariableDeclaration* ScopeManager::getVar(const std::string& varName) const {
    return current->getVar(varName);
}*/

std::unordered_map<std::string, std::shared_ptr<Symbol>>& ScopeManager::GetVars() const {
    return current->GetVars();
}

std::shared_ptr<Symbol> ScopeManager::GetVar(const std::string& name) const {
    return current->GetVar(name);
}

/*
 * Codegen Context
 */

void ScopeManager::pushContext(Context* context) {
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

Context* ScopeManager::getContext() const {
    return contextStack.top();
}
