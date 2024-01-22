#pragma once

#include "../Node.h"
#include "../Visitor.h"
#include "../topLevel/TypeDefinition.h"

class String : public Literal {
public:
    String(const Token& token, const std::string& value)
        : Literal(token),
          value(value) {
    }

    void Accept(class Visitor* visitor) override {
        visitor->Visit(this);
    }

    TypeVariant* getType() override {
        // return Compiler::getScopeManager().getType("string");
        return nullptr;
    }

public:
    const std::string& value;
};
