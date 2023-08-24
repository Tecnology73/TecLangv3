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

    void AddValue(class EnumValue *value);

    const std::vector<class EnumValue *> &GetValues() const;

    int GetValueIndex(const std::string &name) const;

private:
    std::vector<class EnumValue *> values;
    std::map<std::string, int> valueIndices;
};

class EnumValue : public Node {
public:
    EnumValue(Token beginToken) : Node(std::move(beginToken)), name(token.value) {}

    llvm::Value *Accept(Visitor *visitor) override {
        return visitor->Visit(this);
    }

public:
    std::string const name;
    Node *expression = nullptr;
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