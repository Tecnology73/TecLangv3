#pragma once

#include "../Node.h"
#include "../Visitor.h"
#include "VarDeclaration.h"

class ForLoop : public Node {
public:
    using Node::Node;

    ~ForLoop() override {
        delete value;
        delete step;
        delete identifier;

        for (auto &node: body)
            delete node;
    }

    llvm::Value *Accept(Visitor *visitor) override {
        return visitor->Visit(this);
    }

public:
    Node *value = nullptr; // what to loop over (range, array, etc.)
    Node *step = nullptr; // step [expr]
    VariableDeclaration *identifier = nullptr; // as [name] (defaults to `as it`)
    std::vector<Node *> body;
};