#pragma once

#include "TypeBase.h"
#include "../Visitor.h"

class TypeDefinition : public TypeBase {
public:
    using TypeBase::TypeBase;

    void Accept(class Visitor* visitor) override {
        visitor->Visit(this);
    }

    llvm::Value* getDefaultValue() const override;

    bool canCastTo(TypeBase* other) const override;

    /*static TypeVariant* Create(const Token& token);

    static TypeVariant* Create(const Token& token, const std::string& name);

    static TypeVariant* CreateTemporary(const Token& token);*/

    static TypeDefinition* Create(const Token& token);

    static TypeBase* CreateUndeclared(const Token& token);
};
