#pragma once

#include "../Node.h"
#include "../Visitor.h"
#include "../topLevel/TypeDefinition.h"
#include "../../symbolTable/SymbolTable.h"

class Double : public Literal {
public:
    using Literal::Literal;

    double value;

    void Accept(class Visitor* visitor) override {
        visitor->Visit(this);
    }

    TypeVariant* getType() override {
        return std::get<TypeDefinition *>(SymbolTable::GetInstance()->Get("double")->value)->createVariant();
    }
};
