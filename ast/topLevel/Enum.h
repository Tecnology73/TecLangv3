#pragma once

#include "TypeBase.h"
#include "../Visitor.h"

class Enum : public TypeBase {
public:
    using TypeBase::TypeBase;

    void Accept(class Visitor* visitor) override {
        visitor->Visit(this);
    }

    static Enum* Create(const Token& token, const std::string& name);

public:
    TypeReference* underlyingType = nullptr;
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

    EnumParameter(const Token& token, const std::string& name) : Node(token), name(&name) {
    }

    void Accept(Visitor* visitor) override {
        visitor->Visit(this);
    }

public:
    const std::string* name = nullptr; // This is optional
    TypeReference* type = nullptr;
};
