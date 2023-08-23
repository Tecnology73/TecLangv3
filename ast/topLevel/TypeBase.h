#pragma once

#include "../Node.h"

class TypeBase : public Node {
public:
    using Node::Node;

    virtual llvm::Value *getDefaultValue() const = 0;

    virtual bool canCastTo(TypeBase *other) const = 0;

public:
    std::string name;
    llvm::Type *llvmType = nullptr;
    bool isStruct = false;
};