#pragma once

#include "../Node.h"
#include "../Visitor.h"

class ArrayRef : public ChainableNode {
public:
    using ChainableNode::ChainableNode;

    void Accept(Visitor* visitor) override {
        visitor->Visit(this);
    }

    TypeBase* getFinalType() override;

    TypeBase* getFinalType(const TypeBase* parentType) override;

public:
    uint64_t index;
    TypeReference* type = nullptr;
};
