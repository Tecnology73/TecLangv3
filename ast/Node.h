#pragma once

#include <stdexcept>
#include "../lexer/Token.h"

class TypeBase;

class TypeReference;

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
    ChainableNode(const Token& beginToken, const std::string& name) : Node(beginToken), name(name) {
    }

    virtual TypeBase* getFinalType() = 0;

    virtual TypeBase* getFinalType(const TypeBase* parentType) = 0;

    std::string GetText() const override {
        std::string value = name;
        if (next)
            value += "." + next->GetText();

        return value;
    }

public:
    const std::string& name;
    ChainableNode* prev = nullptr;
    ChainableNode* next = nullptr;

    TypeReference* ownerType = nullptr;
    // If true, when we generate the load instructions, we will return the data from inside the struct.
    // If false, we will return the struct itself.
    bool loadInternalData = false;
};

class Literal : public Node {
public:
    using Node::Node;

    void Accept(class Visitor* visitor) override {
        throw std::runtime_error("Literal::Accept() should never be called.");
    }

    virtual TypeReference* getType() = 0;
};
