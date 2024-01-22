#pragma once

#include "../Node.h"
#include "../Visitor.h"
#include "../topLevel/TypeBase.h"

// Used for `new T { field1 = value1, field2 = value2, ... }`
class ConstructorField : public Node {
public:
    ConstructorField(Token beginToken, Node* value) : Node(std::move(beginToken)), name(token.value), value(value) {
    }

    void Accept(Visitor* visitor) override {
    }

public:
    std::string const name;
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

    void AddField(Token token, Node* value);

    static ConstructorCall* Create(const Token& token);

public:
    TypeReference* type;
    // Filled when doing `new T(arg1, arg2, ...)`
    std::vector<Node *> arguments;
    // Filled when doing `new T { field1 = value1, field2 = value2, ... }`
    std::vector<ConstructorField *> fields;
};
