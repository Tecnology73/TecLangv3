#pragma once

#include <iostream>
#include "Node.h"
#include "Visitor.h"

class Literal : public Node {
public:
    using Node::Node;

    llvm::Value *Accept(class Visitor *visitor) override {
        throw std::runtime_error("Literal::Accept() should never be called.");
    }
};

class Integer : public Literal {
public:
    using Literal::Literal;

    int64_t value;
    unsigned numBits = 32;

    Integer(const Token &token, int64_t value) : Literal(token), value(value) {
        numBits = getNumBits();
    }

    llvm::Value *Accept(class Visitor *visitor) override {
        return visitor->Visit(this);
    }

private:
    unsigned int getNumBits() const {
        auto absValue = static_cast<uint64_t>(llabs(value));
        unsigned bits = sizeof(int64_t) * 8 - llvm::countLeadingZeros(absValue);

        if (bits <= 1) return 1;
        if (bits <= 8) return 8;
        if (bits <= 16) return 16;
        if (bits <= 32) return 32;
        return 64;
    }
};

class Double : public Literal {
public:
    using Literal::Literal;

    double value;

    llvm::Value *Accept(class Visitor *visitor) override {
        return visitor->Visit(this);
    }
};

class Boolean : public Literal {
public:
    using Literal::Literal;

    bool value;

    llvm::Value *Accept(class Visitor *visitor) override {
        return visitor->Visit(this);
    }
};

class String : public Literal {
public:
    using Literal::Literal;

    std::string value;

    llvm::Value *Accept(class Visitor *visitor) override {
        return visitor->Visit(this);
    }
};