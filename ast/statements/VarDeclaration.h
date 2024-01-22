#pragma once

#include "../Node.h"
#include "../Visitor.h"
#include "../expressions/TypeReference.h"

class VariableDeclaration : public Node {
public:
    using Node::Node;

    explicit VariableDeclaration(const Token& beginToken, Node* expression = nullptr);

    VariableDeclaration(const Token& beginToken, const std::string& name, Node* expression = nullptr);

    VariableDeclaration(
        const Token& beginToken,
        const std::string& name,
        TypeReference* type,
        Node* expression = nullptr
    );

    ~VariableDeclaration() override;

    void Accept(Visitor* visitor) override;

    // TypeVariant* GetCompiledType(Visitor* visitor);

public:
    std::string const name;
    TypeReference* type = nullptr;
    Node* expression = nullptr;

    llvm::Value* alloc = nullptr;
};
