#pragma once

#include "../Node.h"
#include "../Visitor.h"

class Continue : public Node {
public:
    using Node::Node;

    void Accept(Visitor *visitor) override {
        visitor->Visit(this);
    }
};
