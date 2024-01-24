#pragma once

#include "../Node.h"
#include "../Visitor.h"
#include "../topLevel/TypeDefinition.h"
#include "../../symbolTable/SymbolTable.h"

class Null : public Literal {
public:
    using Literal::Literal;

    void Accept(class Visitor* visitor) override {
        visitor->Visit(this);
    }

    TypeVariant* getType() override {
        return std::get<TypeDefinition *>(SymbolTable::GetInstance()->Get("ptr")->value)->createVariant();
    }
};
