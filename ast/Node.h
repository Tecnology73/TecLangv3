#pragma once

#include <stdexcept>
#include "../lexer/Token.h"

class TypeBase;

class TypeVariant;

class Node {
public:
    explicit Node(const Token& beginToken) : token(beginToken) {
    }

    virtual ~Node() = default;

    virtual void Accept(class Visitor* visitor) = 0;

    Position GetStartPosition() const {
        return token.position;
    }

    virtual Position GetEndPosition() const {
        return token.position;
    }

    virtual std::string GetText() const {
        return token.value.data();
    }

    Token token;
};

class ChainableNode : public Node {
public:
    using Node::Node;

    virtual TypeVariant* getFinalType() = 0;

    virtual TypeVariant* getFinalType(TypeVariant* parentType) = 0;

    std::string GetText() const override {
        std::string value = name;
        if (next)
            value += "." + next->GetText();

        return value;
    }

public:
    std::string name;
    ChainableNode* prev = nullptr;
    ChainableNode* next = nullptr;

    TypeVariant* ownerType = nullptr;
};

class Literal : public Node {
public:
    using Node::Node;

    void Accept(class Visitor* visitor) override {
        throw std::runtime_error("Literal::Accept() should never be called.");
    }

    virtual TypeVariant* getType() = 0;
};
