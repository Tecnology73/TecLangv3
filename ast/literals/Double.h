#pragma once

#include "../Node.h"
#include "../Visitor.h"

class Double : public Literal {
public:
    using Literal::Literal;

    double value;

    void Accept(class Visitor* visitor) override {
        visitor->Visit(this);
    }

    TypeVariant* getType() override {
        return Compiler::getScopeManager().getType("double")->createVariant();
    }
};
