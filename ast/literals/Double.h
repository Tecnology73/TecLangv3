#pragma once

#include "../Node.h"
#include "../Visitor.h"

class Double : public Literal {
public:
    using Literal::Literal;

    double value;

    llvm::Value *Accept(class Visitor *visitor) override {
        return visitor->Visit(this);
    }
};