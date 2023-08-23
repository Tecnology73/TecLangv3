#pragma once

#include <iostream>
#include <unordered_map>
#include <string>
#include <stdexcept>
#include <utility>
#include <llvm/IR/Function.h>
#include "../ast/Node.h"
#include "../ast/Statements.h"
#include "../ast/TopLevel.h"

class ScopeNode {
public:
    explicit ScopeNode(std::string name) : name(std::move(name)) {}

    explicit ScopeNode(std::string name, Function *func) : name(std::move(name)), function(func) {}

    explicit ScopeNode(std::string name, VariableDeclaration *var) : name(std::move(name)), variable(var) {}

    explicit ScopeNode(std::string name, When *when) : name(std::move(name)), when(when) {}

    ~ScopeNode() {
        std::cout << "[Scope] Delete: " << name << std::endl;

        for (auto &item: variables) {
            delete item.second;
        }

        for (auto &item: functions) {
            delete item.second;
        }

        for (auto &item: types) {
            delete item.second;
        }
    }

    bool addVariable(VariableDeclaration *node);

    bool addFunction(Function *node);

    bool addType(TypeDefinition *node);

    bool addEnum(Enum *node);

    bool hasVariable(const std::string &varName) const;

    bool hasFunction(const std::string &funcName) const;

    bool hasType(const std::string &typeName) const;

    bool hasEnum(const std::string &typeName) const;

    VariableDeclaration *lookupVariable(const std::string &varName);

    Function *lookupFunction(const std::string &name);

    TypeDefinition *lookupType(const std::string &typeName);

    TypeDefinition *lookupType(llvm::Type *type) const;

    Enum *lookupEnum(const std::string &enumName) const;

    void setLlvmType(TypeDefinition *type);

    Function *getCurrentFunction() const;

    VariableDeclaration *getCurrentVariable() const;

    When *getCurrentWhen() const;

public:
    ScopeNode *parent = nullptr;
    std::string name;

private:
    std::unordered_map<std::string, VariableDeclaration *> variables;
    std::unordered_map<std::string, Function *> functions;
    std::unordered_map<std::string, TypeDefinition *> types;
    std::unordered_map<std::string, Enum *> enums;
    std::unordered_map<llvm::Type *, TypeDefinition *> compiledTypes;

    Function *function = nullptr;
    VariableDeclaration *variable = nullptr;
    When *when = nullptr;
};

class SymbolTable {
public:
    SymbolTable(llvm::LLVMContext &context);

    void enterScope(std::string name);

    void enterScope(std::string name, Function *function);

    void enterScope(std::string name, VariableDeclaration *var);

    void enterScope(std::string name, When *when);

    void exitScope();

    bool addVariable(VariableDeclaration *node);

    bool addFunction(Function *node);

    bool addType(TypeDefinition *node);

    bool hasVariable(const std::string &varName) const;

    bool hasFunction(const std::string &funcName) const;

    bool hasType(const std::string &typeName) const;

    VariableDeclaration *lookupVariable(const std::string &name) const;

    Function *lookupFunction(const std::string &name) const;

    TypeDefinition *lookupType(const std::string &name) const;

    TypeDefinition *lookupType(llvm::Type *type) const;

    Enum *lookupEnum(const std::string &name) const;

    Function *getCurrentFunction() const;

    VariableDeclaration *getCurrentVariable() const;

    When *getCurrentWhen() const;

    ScopeNode *getRootScope();

private:
    ScopeNode *rootScope;
    ScopeNode *currentScopeNode;

    inline void addBuiltinType(const std::string &name, llvm::LLVMContext &context);
};
