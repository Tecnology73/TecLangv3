#pragma once

#include "../Node.h"
#include "../Visitor.h"
#include "../../compiler/Compiler.h"

class String : public Literal {
public:
    using Literal::Literal;

    std::string value;

    void Accept(class Visitor* visitor) override {
        visitor->Visit(this);
    }

    TypeVariant* getType() override {
        // return Compiler::getScopeManager().getType("string");
        return nullptr;
    }
};
