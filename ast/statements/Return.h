#pragma once

#include "../Node.h"
#include "../Visitor.h"

class Return : public Node {
public:
    using Node::Node;

    ~Return() override {
        delete expression;
    }

    void Accept(Visitor *visitor) override {
        visitor->Visit(this);
    }

public:
    Node *expression = nullptr;
};
