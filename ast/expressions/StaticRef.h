#pragma once

#include "../Node.h"
#include "../Visitor.h"

class StaticRef : public ChainableNode {
public:
    using ChainableNode::ChainableNode;

    llvm::Value *Accept(Visitor *visitor) override {
        return visitor->Visit(this);
    }
};