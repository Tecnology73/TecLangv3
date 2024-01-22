#pragma once

#include "../Node.h"
#include "../Visitor.h"
#include "../topLevel/TypeDefinition.h"
#include "../../symbolTable/SymbolTable.h"

class Boolean : public Literal {
public:
    using Literal::Literal;

    bool value;

    void Accept(class Visitor* visitor) override {
        visitor->Visit(this);
    }

    TypeVariant* getType() override {
        return std::get<TypeDefinition *>(SymbolTable::GetInstance()->Get("bool")->value)->createVariant();
    }
};
