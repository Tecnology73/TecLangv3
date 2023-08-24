#pragma once

#include "../Node.h"
#include "../Visitor.h"

class Break : public Node {
public:
    using Node::Node;

    llvm::Value *Accept(Visitor *visitor) override {
        return visitor->Visit(this);
    }
};