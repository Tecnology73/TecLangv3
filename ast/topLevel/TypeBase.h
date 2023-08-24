#pragma once

#include "../Node.h"

class TypeBase : public Node {
public:
    TypeBase(const Token &token, std::string name) : Node(token), name(std::move(name)) {}

    virtual llvm::Value *getDefaultValue() const = 0;

    virtual bool canCastTo(TypeBase *other) const = 0;

    llvm::Type *getLlvmType();

public:
    std::string const name;

public:
    llvm::Type *llvmType = nullptr;
    bool isValueType = true;
};