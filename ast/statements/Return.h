#pragma once

#include "../Node.h"
#include "../Visitor.h"

class Return : public Node {
public:
    using Node::Node;

    ~Return() override {
        delete expression;
    }

    llvm::Value *Accept(Visitor *visitor) override {
        return visitor->Visit(this);
    }

public:
    Node *expression = nullptr;
};