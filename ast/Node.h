#pragma once

#include <stdexcept>
#include <llvm/IR/Value.h>
#include "../lexer/Token.h"

class Node {
public:
    explicit Node(const Token &beginToken) : token(beginToken) {}

    virtual ~Node() = default;

    virtual llvm::Value *Accept(class Visitor *visitor) = 0;

    Position GetStartPosition() const {
        return token.position;
    }

    virtual Position GetEndPosition() const {
        return token.position;
    }

    Token token;
};

class ChainableNode : public Node {
public:
    using Node::Node;

    std::string name;
    ChainableNode *prev = nullptr;
    ChainableNode *next = nullptr;

    class TypeDefinition *ownerType = nullptr;
};

class Literal : public Node {
public:
    using Node::Node;

    llvm::Value *Accept(class Visitor *visitor) override {
        throw std::runtime_error("Literal::Accept() should never be called.");
    }
};
