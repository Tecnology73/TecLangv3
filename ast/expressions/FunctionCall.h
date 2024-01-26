#pragma once

#include "../Node.h"
#include "../Visitor.h"

class FunctionCall : public ChainableNode {
public:
    using ChainableNode::ChainableNode;

    void Accept(Visitor* visitor) override {
        visitor->Visit(this);
    }

    TypeBase* getFinalType() override;

    TypeBase* getFinalType(const TypeBase* parentType) override;

public:
    std::vector<Node *> arguments;
    // During one of the early analysis stages, we lookup the function this
    // is calling and save it so we don't have to keep performing lookups.
    Function* function = nullptr;
};
