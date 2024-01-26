#pragma once

#include "../Node.h"
#include "../Visitor.h"

class IfStatement : public Node {
public:
    using Node::Node;

    ~IfStatement() override {
        delete condition;
        delete elseStatement;

        for (const auto& node: body)
            delete node;
    }

    void Accept(Visitor* visitor) override {
        visitor->Visit(this);
    }

public:
    Node* condition = nullptr;
    std::vector<Node *> body;
    IfStatement* elseStatement = nullptr;
};
