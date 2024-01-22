#pragma once

#include "../Node.h"
#include "../Visitor.h"
#include "../../compiler/Compiler.h"

class Boolean : public Literal {
public:
    using Literal::Literal;

    bool value;

    void Accept(class Visitor* visitor) override {
        visitor->Visit(this);
    }

    TypeVariant* getType() override {
        return Compiler::getScopeManager().getType("bool")->createVariant();
    }
};
