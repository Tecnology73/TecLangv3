#pragma once

#include "TypeBase.h"
#include "../Visitor.h"

class Enum : public TypeBase {
public:
    using TypeBase::TypeBase;

    std::vector<class EnumValue *> values;

    llvm::Value *Accept(class Visitor *visitor) override {
        return visitor->Visit(this);
    }

    llvm::Value *getDefaultValue() const override;

    bool canCastTo(TypeBase *other) const override;

    void AddValue(class EnumValue *value);

    int GetValueIndex(const std::string &name) const;

private:
    std::map<std::string, int> valueIndices;
};

class EnumValue : public Node {
public:
    using Node::Node;

    std::string name;
    Node *expression = nullptr;

    llvm::Value *Accept(class Visitor *visitor) override {
        return visitor->Visit(this);
    }
};

class EnumConstructor : public EnumValue {
public:
    using EnumValue::EnumValue;

    std::vector<class EnumParameter *> parameters;

    llvm::Value *Accept(class Visitor *visitor) override {
        return visitor->Visit(this);
    }
};

class EnumParameter : public Node {
public:
    using Node::Node;

    std::string name;
    TypeDefinition *type;

    llvm::Value *Accept(class Visitor *visitor) override {
        return visitor->Visit(this);
    }
};