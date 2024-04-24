#pragma once

#include "TypeBase.h"
#include "Function.h"
#include "../Visitor.h"
#include "../StringInternTable.h"

class Enum : public TypeBase {
public:
    using TypeBase::TypeBase;

    void Accept(class Visitor* visitor) override {
        visitor->Visit(this);
    }

    static Enum* Create(const Token& token, const std::string& name);

    TypeReference* CreateConstructorType(EnumConstructor* constructor) const;

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

class EnumConstructor : public Function {
public:
    using Function::Function;

    void Accept(Visitor* visitor) override {
        visitor->Visit(this);
    }
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
