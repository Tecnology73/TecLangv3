#pragma once

#include "unordered_map"
#include "../../ast/TopLevel.h"
#include "../../ast/Statements.h"

class TypeVariant;

class Symbol {
public:
    explicit Symbol(const VariableDeclaration* var) : node(var) {
        narrowedType = var->type->ResolveType()->Clone();
    }

    explicit Symbol(const Node* node, const TypeVariant* type) : node(node), narrowedType(type->Clone()) {
    }

    explicit Symbol(const TypeField* field) : node(field) {
        narrowedType = field->type->ResolveType()->Clone();
    }

    std::shared_ptr<Symbol> GetField(const std::string& name) {
        if (auto field = fields.find(name); field != fields.end())
            return field->second;

        auto field = narrowedType->type->GetField(name);
        if (!field)
            return nullptr;

        return fields[name] = std::make_shared<Symbol>(field);
    }

public:
    // This holds the original type
    const Node* node;
    // This is the narrowed type. It changes as we go down the AST.
    TypeVariant* narrowedType;

    std::unordered_map<std::string, std::shared_ptr<Symbol>> fields;
};

class Scope {
    friend class ScopeManager;

public:
    const std::string name;

// protected:
    explicit Scope(std::string name) : name(std::move(name)) {
    }

    /*void add(VariableDeclaration* var) {
        vars[var->name] = var;
    }

    bool hasVar(const std::string& varName) const {
        if (vars.find(varName) != vars.end())
            return true;

        if (parent)
            return parent->hasVar(varName);

        return false;
    }

    VariableDeclaration* getVar(const std::string& varName) const {
        auto it = vars.find(varName);
        if (it != vars.end())
            return it->second;

        if (parent)
            return parent->getVar(varName);

        return nullptr;
    }*/

    void Add(const VariableDeclaration* var) {
        vars[var->name] = std::make_shared<Symbol>(var);
    }

    void Add(const std::string& name, const Node* node, const TypeVariant* narrowedType) {
        vars[name] = std::make_shared<Symbol>(node, narrowedType);
    }

    bool HasVar(const std::string& varName) const {
        if (vars.find(varName) != vars.end())
            return true;

        if (parent)
            return parent->HasVar(varName);

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
    Scope* parent = nullptr;

    // std::unordered_map<std::string, VariableDeclaration *> vars;
    std::unordered_map<std::string, std::shared_ptr<Symbol>> vars;
};

//
// --------------------------------------------------------------------------------
//

class RootScope : public Scope {
    friend class ScopeManager;

protected:
    using Scope::Scope;

    void add(TypeDefinition* typeDef) {
        types[typeDef->name] = typeDef;

        if (typeDef->isDeclared)
            migrateUndeclaredUses(typeDef);
    }

    void add(Enum* anEnum) {
        types[anEnum->name] = anEnum;

        if (anEnum->isDeclared)
            migrateUndeclaredUses(anEnum);
    }

    void add(Function* function) { functions[std::string(function->name)] = function; }

    void addCompiledType(TypeDefinition* typeDef) { compiledTypes[typeDef->llvmType] = typeDef; }

    bool hasType(const std::string& typeName) const { return types.contains(typeName); }

    bool hasEnum(const std::string& enumName) const { return types.contains(enumName); }

    bool hasFunction(const std::string& funcName) const { return functions.contains(funcName); }

    TypeBase* getType(const std::string& typeName, bool onlyDeclared = false) const {
        const auto it = types.find(typeName);
        if (it == types.end())
            return nullptr;

        const auto type = dynamic_cast<TypeBase *>(it->second);
        if (!type || (onlyDeclared && !type->isDeclared))
            return nullptr;

        return type;
    }

    TypeDefinition* getType(const llvm::Type* llvmType) const {
        const auto it = compiledTypes.find(llvmType);
        if (it == compiledTypes.end())
            return nullptr;

        if (const auto type = dynamic_cast<TypeDefinition *>(it->second))
            return type;

        return nullptr;
    }

    Enum* getEnum(const std::string& enumName, bool onlyDeclared = false) const {
        const auto it = types.find(enumName);
        if (it == types.end())
            return nullptr;

        const auto anEnum = dynamic_cast<Enum *>(it->second);
        if (!anEnum || (onlyDeclared && !anEnum->isDeclared))
            return nullptr;

        return anEnum;
    }

    Function* getFunction(const std::string& funcName) const {
        auto it = functions.find(funcName);
        if (it != functions.end())
            return it->second;

        return nullptr;
    }

    void addTypeUse(TypeVariant* variant, const TypeBase* type) {
        if (type->isDeclared) return;

        if (!undeclaredTypeUses.contains(variant->type->name))
            undeclaredTypeUses[variant->type->name] = {};

        undeclaredTypeUses[variant->type->name].push_back(variant);
    }

    void migrateUndeclaredUses(TypeBase* type) {
        if (!undeclaredTypeUses.contains(type->name)) return;

        for (auto& variant: undeclaredTypeUses[type->name])
            variant->SetType(type);

        undeclaredTypeUses.erase(type->name);
    }

private:
    std::unordered_map<std::string, TypeBase *> types;
    std::unordered_map<const llvm::Type *, TypeDefinition *> compiledTypes;
    std::unordered_map<std::string, Function *> functions;

    std::unordered_map<std::string, std::vector<TypeVariant *>> undeclaredTypeUses;
};
