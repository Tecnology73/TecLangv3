#pragma once

#include <llvm/IR/Value.h>
#include "../lexer/Token.h"

class Node {
public:
    explicit Node(Token beginToken) : token(std::move(beginToken)) {}

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