#pragma once

#include "TypeBase.h"
#include "../Visitor.h"

class Enum : public TypeBase {
public:
    using TypeBase::TypeBase;

    void Accept(class Visitor* visitor) override {
        visitor->Visit(this);
    }

    llvm::Value* getDefaultValue() const override;

    bool canCastTo(TypeBase* other) const override;

    static Enum* Create(const Token& token, const std::string& name);
};

class EnumValue : public TypeField {
public:
    using TypeField::TypeField;

    void Accept(Visitor* visitor) override {
        visitor->Visit(this);
    }
};

class EnumConstructor : public EnumValue {
public:
    using EnumValue::EnumValue;

    void Accept(Visitor* visitor) override {
        visitor->Visit(this);
    }

public:
    std::vector<class EnumParameter *> parameters;
};

class EnumParameter : public Node {
public:
    using Node::Node;

    void Accept(Visitor* visitor) override {
        visitor->Visit(this);
    }

public:
    std::string name; // This is optional
    TypeReference* type;
};
