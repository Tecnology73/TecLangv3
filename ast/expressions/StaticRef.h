#pragma once

#include "../Node.h"
#include "../Visitor.h"

class StaticRef : public ChainableNode {
public:
    using ChainableNode::ChainableNode;

    explicit StaticRef(const Token& beginToken) : ChainableNode(beginToken) {
        name = beginToken.value;
    }

    void Accept(Visitor* visitor) override {
        visitor->Visit(this);
    }

    TypeVariant* getFinalType() override;

    TypeVariant* getFinalType(TypeVariant* parentType) override;
};
