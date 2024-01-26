#include "ScopeManager.h"
#include <llvm/IR/DerivedTypes.h>

ScopeManager* ScopeManager::instance = nullptr;

ScopeManager::ScopeManager() : current(new Scope("root")) {
    instance = this;
}

ScopeManager* ScopeManager::get() {
    return instance;
}

void ScopeManager::leave(const std::string& name) {
    if (current->name != name) {
        printError("Cannot leave scope '%s' because current scope is '%s'!", name.c_str(), current->name.c_str());
        return;
    }

    if (current->parent != nullptr) {
        // printDebug("[Scope] Leave: %s < %s", current->parent->name.c_str(), current->name.c_str());
        current = current->parent;
    }
}

std::shared_ptr<Scope> ScopeManager::GetParentScope(unsigned levels) const {
    auto scope = current;
    while (levels-- > 0 && scope->parent != nullptr)
        scope = scope->parent;

    return scope;
}

/*
 * Add
 */

void ScopeManager::Add(const VariableDeclaration* var) const {
    if (!current->parent) {
        // TODO: Use the ErrorManager
        printError("[Scope] Cannot add variable to root scope!");
        return;
    }

    current->Add(var);
}

void ScopeManager::Add(const std::string& name, const Node* node, const TypeReference* narrowedType) const {
    if (!current->parent) {
        // TODO: Use the ErrorManager
        printError("[Scope] Cannot add variable to root scope!");
        return;
    }

    current->Add(name, node, narrowedType);
}

/*
 * Has
 */

bool ScopeManager::HasVar(const std::string& name) const {
    return current->HasVar(name);
}

/*
 * Get
 */

std::unordered_map<std::string, std::shared_ptr<Symbol>>& ScopeManager::GetVars() const {
    return current->GetVars();
}

std::shared_ptr<Symbol> ScopeManager::GetVar(const std::string& name) const {
    return current->GetVar(name);
}

/*
 * Context
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
