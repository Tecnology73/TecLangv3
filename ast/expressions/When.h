#pragma once

#include "../Node.h"
#include "../Visitor.h"

class When : public Node {
public:
    using Node::Node;

    llvm::Value *Accept(Visitor *visitor) override {
        return visitor->Visit(this);
    }

public:
    Node *condition;
    std::vector<class WhenCondition *> body;

    // TODO: This is a messy. Fix it.
    bool hasElse = false;
    bool areAllCasesCovered = false;
};

class WhenCondition : public Node {
public:
    using Node::Node;

    llvm::Value *Accept(Visitor *visitor) override {
        return visitor->Visit(this);
    }

public:
    Node *condition = nullptr; // null if it's an else.
    std::vector<Node *> body;
    // This is set to true when there are no curly braces around the body.
    bool implicitReturn = false;
};