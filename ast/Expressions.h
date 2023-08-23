#pragma once

#include <string>
#include <vector>
#include <map>
#include "Node.h"
#include "Visitor.h"

class ConstructorCall : public Node {
public:
    using Node::Node;

    TypeDefinition *type;
    // Filled when doing `new T(arg1, arg2, ...)`
    std::vector<Node *> arguments;
    // Filled when doing `T { field1 = value1, field2 = value2, ... }`
    std::vector<VariableDeclaration *> assignments;

    llvm::Value *Accept(class Visitor *visitor) override {
        return visitor->Visit(this);
    }

    Position GetEndPosition() const override;

    static ConstructorCall *Create(const Token &token);
};

class FunctionCall : public ChainableNode {
public:
    using ChainableNode::ChainableNode;

    std::vector<Node *> arguments;

    llvm::Value *Accept(class Visitor *visitor) override {
        return visitor->Visit(this);
    }
};

class BinaryOperation : public Node {
public:
    using Node::Node;

    Node *lhs;
    Node *rhs;
    Token::Type op;

    llvm::Value *Accept(class Visitor *visitor) override {
        return visitor->Visit(this);
    }

    static BinaryOperation *Create(const Token &token, Node *lhs, Node *rhs);
};

class VariableReference : public ChainableNode {
public:
    using ChainableNode::ChainableNode;

    llvm::Value *Accept(class Visitor *visitor) override {
        return visitor->Visit(this);
    }
};

class StaticRef : public ChainableNode {
public:
    using ChainableNode::ChainableNode;

    llvm::Value *Accept(class Visitor *visitor) override {
        return visitor->Visit(this);
    }
};

class When : public Node {
public:
    using Node::Node;

    Node *condition;
    std::vector<class WhenCondition *> body;

    bool hasElse = false;
    bool areAllCasesCovered = false;
    llvm::BasicBlock *exitBlock = nullptr;

    llvm::Value *Accept(class Visitor *visitor) override {
        return visitor->Visit(this);
    }
};

class WhenCondition : public Node {
public:
    using Node::Node;

    Node *condition = nullptr; // nullptr if it's an else.
    std::vector<Node *> body;
    // This is set to true when there are no curly braces around the body.
    bool implicitReturn = false;

    llvm::Value *Accept(class Visitor *visitor) override {
        return visitor->Visit(this);
    }
};