#pragma once

#include "../Node.h"
#include "../Visitor.h"

class IfStatement : public Node {
public:
    using Node::Node;

    ~IfStatement() override {
        delete condition;
        delete elseStatement;

        for (auto &node: body)
            delete node;
    }

    llvm::Value *Accept(Visitor *visitor) override {
        return visitor->Visit(this);
    }

public:
    Node *condition = nullptr;
    std::vector<Node *> body;
    IfStatement *elseStatement = nullptr;
};