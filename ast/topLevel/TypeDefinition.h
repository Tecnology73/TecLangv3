#pragma once

#include "TypeBase.h"
#include "../Expressions.h"

class TypeDefinition : public TypeBase {
public:
    using TypeBase::TypeBase;

    llvm::Value *Accept(class Visitor *visitor) override {
        visitor->Visit(this);
        return nullptr;
    }

    llvm::Value *getDefaultValue() const override;

    bool canCastTo(TypeBase *other) const override;

    static TypeDefinition *Create(const Token &token);

    static TypeDefinition *Create(const Token &token, std::string name);
};
