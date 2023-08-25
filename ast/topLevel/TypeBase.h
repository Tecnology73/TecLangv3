#pragma once

#include <map>
#include <vector>
#include <llvm/IR/DerivedTypes.h>
#include "../Node.h"
#include "../Visitor.h"

class Function;

class TypeField;

class TypeBase : public Node {
public:
    TypeBase(const Token &token, std::string name) : Node(token), name(std::move(name)) {}

    virtual llvm::Value *getDefaultValue() const = 0;

    virtual bool canCastTo(TypeBase *other) const = 0;

    llvm::Type *getLlvmType();

    llvm::Type *setLlvmType(llvm::Type *type);

    void AddField(TypeField *field);

    void AddFunction(Function *function);

    TypeField *GetField(const std::string &fieldName) const;

    int GetFieldIndex(const std::string &fieldName) const;

    Function *GetFunction(const std::string &funcName) const;

    Function *FindFunction(const std::string &funcName, const std::vector<llvm::Value *> &parameters) const;

public:
    std::string const name;
    // Because we allow overloading, we need to store an array of functions.
    std::unordered_map<std::string, std::vector<Function *>> functions;
    std::unordered_map<std::string, TypeField *> fields;
    std::vector<std::string> fieldOrder;

    llvm::Type *llvmType = nullptr;
    bool isValueType = true;
};

class TypeField : public Node {
public:
    TypeField(const Token &token, const std::string &name) : Node(token), name(name) {}

    llvm::Value *Accept(class Visitor *visitor) override {
        return visitor->Visit(this);
    }

public:
    std::string const name;
    TypeBase *type = nullptr; // Empty if we need to infer the type.
    Node *expression = nullptr;
    int index = 0;
};
