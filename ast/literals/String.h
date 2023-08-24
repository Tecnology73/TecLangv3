#pragma once

#include "../Node.h"
#include "../Visitor.h"

class String : public Literal {
public:
    using Literal::Literal;

    std::string value;

    llvm::Value *Accept(class Visitor *visitor) override {
        return visitor->Visit(this);
    }
};