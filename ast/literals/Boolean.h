#pragma once

#include "../Node.h"
#include "../Visitor.h"

class Boolean : public Literal {
public:
    using Literal::Literal;

    bool value;

    llvm::Value *Accept(class Visitor *visitor) override {
        return visitor->Visit(this);
    }
};