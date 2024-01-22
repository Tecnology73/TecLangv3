#pragma once

#include "../Node.h"
#include "../Visitor.h"
#include "../../compiler/Compiler.h"

class Null : public Literal {
public:
    using Literal::Literal;

    void Accept(class Visitor* visitor) override {
        visitor->Visit(this);
    }

    TypeVariant* getType() override {
        return Compiler::getScopeManager().getType("null")->createVariant();
    }
};
