#pragma once

#include "../Node.h"
#include "../Visitor.h"

class When : public Node {
public:
    using Node::Node;

    void Accept(Visitor* visitor) override {
        visitor->Visit(this);
    }

public:
    Node* expression;
    std::vector<class WhenCondition *> body;
    TypeReference* returnType = nullptr;

    // TODO: This is a messy. Fix it.
    bool hasElse = false;
    bool areAllCasesCovered = false;
};

class WhenCondition : public Node {
public:
    using Node::Node;

    void Accept(Visitor* visitor) override {
        visitor->Visit(this);
    }

public:
    Node* condition = nullptr; // null if it's an else.
    std::vector<Node *> body;
    // This is set to true when there are no curly braces around the body.
    bool implicitReturn = false;
};
