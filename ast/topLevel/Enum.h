#pragma once

#include "TypeBase.h"
#include "../Visitor.h"

class Enum : public TypeBase {
public:
    using TypeBase::TypeBase;

    llvm::Value *Accept(Visitor *visitor) override {
        return visitor->Visit(this);
    }

    llvm::Value *getDefaultValue() const override;

    bool canCastTo(TypeBase *other) const override;

    static Enum *Create(const Token &token, std::string name);
};

class EnumValue : public TypeField {
public:
    using TypeField::TypeField;

    llvm::Value *Accept(Visitor *visitor) override {
        return visitor->Visit(this);
    }
};

class EnumConstructor : public EnumValue {
public:
    using EnumValue::EnumValue;

    llvm::Value *Accept(Visitor *visitor) override {
        return visitor->Visit(this);
    }

public:
    std::vector<class EnumParameter *> parameters;
};

class EnumParameter : public Node {
public:
    using Node::Node;

    llvm::Value *Accept(Visitor *visitor) override {
        return visitor->Visit(this);
    }

public:
    std::string name; // This is optional
    TypeBase *type;
};
