#pragma once

#include "unordered_map"
#include "../../ast/TopLevel.h"
#include "../../ast/Statements.h"
#include "../../ast/Expressions.h"

class Scope {
    friend class ScopeManager;

public:
    const std::string name;

protected:
    explicit Scope(std::string name) : name(std::move(name)) {}

    void add(VariableDeclaration *var) { vars[var->name] = var; }

    bool hasVar(const std::string &varName) const {
        if (vars.find(varName) != vars.end())
            return true;

        if (parent)
            return parent->hasVar(varName);

        return false;
    }

    VariableDeclaration *getVar(const std::string &varName) const {
        auto it = vars.find(varName);
        if (it != vars.end())
            return it->second;

        if (parent)
            return parent->getVar(varName);

        return nullptr;
    }

private:
    Scope *parent = nullptr;

    std::unordered_map<std::string, VariableDeclaration *> vars;
};

//
// --------------------------------------------------------------------------------
//

class RootScope : public Scope {
    friend class ScopeManager;

protected:
    using Scope::Scope;

    void add(TypeDefinition *typeDef) { types[typeDef->name] = typeDef; }

    void add(Enum *anEnum) { enums[anEnum->name] = anEnum; }

    void add(Function *function) { functions[function->name] = function; }

    void addCompiledType(TypeDefinition *typeDef) { compiledTypes[typeDef->llvmType] = typeDef; }

    bool hasType(const std::string &typeName) const { return types.find(typeName) != types.end(); }

    bool hasEnum(const std::string &enumName) const { return enums.find(enumName) != enums.end(); }

    bool hasFunction(const std::string &funcName) const { return functions.find(funcName) != functions.end(); }

    TypeDefinition *getType(const std::string &typeName) const {
        auto it = types.find(typeName);
        if (it != types.end())
            return it->second;

        return nullptr;
    }

    TypeDefinition *getType(llvm::Type *llvmType) const {
        auto it = compiledTypes.find(llvmType);
        if (it != compiledTypes.end())
            return it->second;

        return nullptr;
    }

    Enum *getEnum(const std::string &enumName) const {
        auto it = enums.find(enumName);
        if (it != enums.end())
            return it->second;

        return nullptr;
    }

    Function *getFunction(const std::string &funcName) const {
        auto it = functions.find(funcName);
        if (it != functions.end())
            return it->second;

        return nullptr;
    }

private:
    std::unordered_map<std::string, TypeDefinition *> types;
    std::unordered_map<std::string, Enum *> enums;
    std::unordered_map<std::string, Function *> functions;
    std::unordered_map<llvm::Type *, TypeDefinition *> compiledTypes;
};