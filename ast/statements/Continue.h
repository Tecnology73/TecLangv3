#pragma once

#include "../Node.h"
#include "../Visitor.h"

class Continue : public Node {
public:
    using Node::Node;

    llvm::Value *Accept(Visitor *visitor) override {
        return visitor->Visit(this);
    }
};