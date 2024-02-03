#pragma once

#include "TypeBase.h"
#include "../Visitor.h"

class TypeDefinition : public TypeBase {
public:
    using TypeBase::TypeBase;

    void Accept(class Visitor* visitor) override {
        visitor->Visit(this);
    }

    static TypeDefinition* Create(const Token& token);

    static TypeBase* CreateUndeclared(const Token& token);
};
