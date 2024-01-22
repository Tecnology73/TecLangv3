#pragma once

#include "../Node.h"
#include "../Visitor.h"

class BinaryOperation : public Node {
public:
    using Node::Node;

    BinaryOperation(Token beginToken, Node *lhs, Node *rhs) : Node(std::move(beginToken)), lhs(lhs), rhs(rhs), op(token.type) {}

    void Accept(Visitor *visitor) override {
        visitor->Visit(this);
    }

public:
    Node *const lhs;
    Node *const rhs;
    Token::Type const op;
};
