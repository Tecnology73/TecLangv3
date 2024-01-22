#pragma once

#include "../Node.h"
#include "../Visitor.h"
#include "../topLevel/TypeBase.h"

// Used for `new T { field1 = value1, field2 = value2, ... }`
class ConstructorField : public Node {
public:
    ConstructorField(const Token& beginToken, const std::string& name, Node* value)
        : Node(beginToken),
          name(name),
          value(value) {
    }

    void Accept(Visitor* visitor) override {
    }

public:
    const std::string& name;
    Node* const value;
};

class ConstructorCall : public Node {
public:
    using Node::Node;

    void Accept(Visitor* visitor) override {
        visitor->Visit(this);
    }

    Position GetEndPosition() const override;

    void AddArgument(Node* arg);

    void AddField(const Token& token, Node* value);

public:
    TypeReference* type;
    // Filled when doing `new T(arg1, arg2, ...)`
    std::vector<Node *> arguments;
    // Filled when doing `new T { field1 = value1, field2 = value2, ... }`
    std::vector<ConstructorField *> fields;
};
