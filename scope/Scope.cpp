#include "Scope.h"
#include "../compiler/statements/VariableDeclaration.h"

std::shared_ptr<Scope> Scope::current = nullptr;

Scope::Scope(std::shared_ptr<Scope> parent, std::shared_ptr<Context> context) : parent(parent), context(context) {
}

std::shared_ptr<Scope> Scope::GetScope() {
    return current;
}

std::shared_ptr<Context> Scope::GetContext() {
    if (!current)
        return nullptr;

    return current->context;
}

std::shared_ptr<Scope> Scope::GetParent() {
    if (!current)
        return nullptr;

    return current->parent;
}

std::shared_ptr<Scope> Scope::Leave() {
    return current = parent;
}

void Scope::Add(VariableDeclaration* node) {
    vars[node->name] = std::make_shared<Symbol>(node, node->type);
}

std::shared_ptr<Symbol> Scope::Get(const std::string& name) const {
    if (auto it = vars.find(name); it != vars.end())
        return it->second;

    if (parent)
        return parent->Get(name);

    return nullptr;
}

std::unordered_map<std::string, std::shared_ptr<Symbol>>& Scope::List() {
    return vars;
}

bool Scope::Has(const std::string& name) const {
    return Get(name) != nullptr;
}
