#pragma once

#include "../Node.h"
#include "../Visitor.h"
#include "../topLevel/TypeBase.h"

class VariableDeclaration : public Node {
public:
    using Node::Node;

    VariableDeclaration(const Token &beginToken, Node *expression = nullptr);

    VariableDeclaration(const Token &beginToken, const std::string &name, Node *expression = nullptr);

    ~VariableDeclaration() override;

    llvm::Value *Accept(Visitor *visitor) override;

    TypeBase *GetCompiledType(Visitor *visitor);

public:
    std::string const name;
    TypeBase *type = nullptr;
    Node *expression = nullptr;

    llvm::Value *alloc = nullptr;
};
