#pragma once

#include "../Node.h"
#include "../Visitor.h"

class FunctionCall : public ChainableNode {
public:
    using ChainableNode::ChainableNode;

    void Accept(Visitor* visitor) override {
        visitor->Visit(this);
    }

    TypeVariant* getFinalType() override;

    TypeVariant* getFinalType(TypeVariant* parentType) override;

public:
    std::vector<Node *> arguments;
};
