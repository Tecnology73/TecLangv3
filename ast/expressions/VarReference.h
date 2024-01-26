#pragma once

#include "../Node.h"
#include "../Visitor.h"

class VariableReference : public ChainableNode {
public:
    using ChainableNode::ChainableNode;

    void Accept(Visitor* visitor) override {
        visitor->Visit(this);
    }

    TypeBase* getFinalType() override;

    TypeBase* getFinalType(const TypeBase* parentType) override;

public:
    TypeReference* type = nullptr;
};
