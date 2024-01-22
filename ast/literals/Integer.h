#pragma once

#include "../Node.h"
#include "../Visitor.h"
#include "../topLevel/TypeDefinition.h"
#include "../../symbolTable/SymbolTable.h"

class Integer : public Literal {
public:
    using Literal::Literal;

    int64_t value;
    unsigned numBits = 32;

    Integer(const Token& token, const int64_t value) : Literal(token), value(value) {
        numBits = getNumBits();
    }

    void Accept(Visitor* visitor) override {
        visitor->Visit(this);
    }

    TypeVariant* getType() override {
        if (numBits == 1)
            return std::get<TypeDefinition *>(SymbolTable::GetInstance()->Get("i8")->value)->createVariant();

        return std::get<TypeDefinition *>(
            SymbolTable::GetInstance()->Get("i" + std::to_string(numBits))->value
        )->createVariant();
    }

private:
    unsigned int getNumBits() const {
        // I don't understand why but LLVM requires i32
        // to properly represent negative numbers in the IR.
        if (value < 0) return 32;

        auto absValue = static_cast<uint64_t>(llabs(value));
        unsigned bits = sizeof(int64_t) * 8 - llvm::countLeadingZeros(absValue);

        if (bits <= 1) return 1;
        if (bits <= 8) return 8;
        if (bits <= 16) return 16;
        if (bits <= 32) return 32;

        return 64;
    }
};
