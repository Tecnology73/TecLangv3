#pragma once

#include "unordered_map"
#include "../../ast/TopLevel.h"

class Symbol {
public:
    explicit Symbol(const VariableDeclaration* var) : node(var) {
        narrowedType = var->type->Clone();
    }

    explicit Symbol(const Node* node, const TypeReference* type) : node(node), narrowedType(type->Clone()) {
    }

    explicit Symbol(const TypeField* field) : node(field) {
        narrowedType = field->type->Clone();
    }

    std::shared_ptr<Symbol> GetField(const std::string& name) {
        if (auto field = fields.find(name); field != fields.end())
            return field->second;

        auto field = narrowedType->ResolveType()->GetField(name);
        if (!field)
            return nullptr;

        return fields[name] = std::make_shared<Symbol>(field);
    }

public:
    // This holds the original type
    const Node* node;
    // This is the narrowed type. It changes as we go down the AST.
    TypeReference* narrowedType;

    std::unordered_map<std::string, std::shared_ptr<Symbol>> fields;
};

class Scope {
    friend class ScopeManager;

public:
    const std::string name;

// protected:
    explicit Scope(std::string name) : name(std::move(name)) {
    }

    void Add(const VariableDeclaration* var) {
        vars[var->name] = std::make_shared<Symbol>(var);
    }

    void Add(const std::string& name, const Node* node, const TypeReference* narrowedType) {
        vars[name] = std::make_shared<Symbol>(node, narrowedType);
    }

    bool HasVar(const std::string& name) const {
        if (vars.contains(name))
            return true;

        if (parent)
            return parent->HasVar(name);

        return false;
    }

    std::unordered_map<std::string, std::shared_ptr<Symbol>>& GetVars() {
        return vars;
    }

    std::shared_ptr<Symbol> GetVar(const std::string& name) const {
        if (auto it = vars.find(name); it != vars.end())
            return it->second;

        if (parent)
            return parent->GetVar(name);

        return nullptr;
    }

private:
    std::shared_ptr<Scope> parent = nullptr;
    std::unordered_map<std::string, std::shared_ptr<Symbol>> vars;
};
