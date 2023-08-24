#pragma once

#include "../Node.h"
#include "../Visitor.h"

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