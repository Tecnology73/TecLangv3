#pragma once

#include "../Node.h"
#include "../Visitor.h"
#include "../topLevel/TypeBase.h"
// #include "../../compiler/Compiler.h"

class VariableDeclaration : public Node {
public:
    using Node::Node;

    VariableDeclaration(Token beginToken, Node *expression = nullptr) : Node(std::move(beginToken)), name(token.value), expression(expression) {}

    VariableDeclaration(Token beginToken, std::string name, Node *expression = nullptr) : Node(std::move(beginToken)), name(std::move(name)), expression(expression) {}

    ~VariableDeclaration() override {
        delete expression;
    }

    llvm::Value *Accept(Visitor *visitor) override {
        return visitor->Visit(this);
    }

    TypeDefinition *GetCompiledType(Visitor *visitor) {
        /*if (!type) {
            type = inferType(v, this);
            if (!type) return nullptr;
        }

        if (!type->llvmType) {
            if (auto typeDef = dynamic_cast<TypeDefinition *>(type))
                generateTypeDefinition(visitor, typeDef);
            else
                type->llvmType = Compiler::getScopeManager().getType("i32")->llvmType;
        }

        return dynamic_cast<TypeDefinition *>(type);*/
        return nullptr;
    }

public:
    std::string const name;
    TypeBase *type = nullptr;
    Node *expression = nullptr;

    llvm::Value *alloc = nullptr;
};