#pragma once

#include <string>
#include <vector>
#include <map>
#include <llvm/IR/DerivedTypes.h>
#include "Node.h"
#include "Visitor.h"
#include "Expressions.h"
//
#include "topLevel/TypeBase.h"
#include "topLevel/TypeDefinition.h"

class Function : public Node {
public:
    using Node::Node;

    Function() : Node(Token{}), isExternal(true) {}

    std::string name;
    std::vector<FunctionParameter *> parameters;
    std::map<std::string, int> parameterIndices;
    std::vector<Node *> body;
    TypeDefinition *returnType = nullptr;
    TypeDefinition *ownerType = nullptr;
    bool isExternal = false;

    llvm::Function *llvmFunction = nullptr;

    llvm::Value *Accept(class Visitor *visitor) override {
        return visitor->Visit(this);
    }

    void addParameter(FunctionParameter *parameter);

    FunctionParameter *getParameter(const std::string &parameterName) const;

    int getParameterIndex(const std::string &parameterName);
};

class Return : public Node {
public:
    using Node::Node;

    Node *expression = nullptr;

    ~Return() override {
        delete expression;
    }

    llvm::Value *Accept(class Visitor *visitor) override {
        return visitor->Visit(this);
    }
};

class VariableDeclaration : public Node {
public:
    using Node::Node;

    std::string name;
    TypeBase *type = nullptr;
    Node *expression = nullptr;
    llvm::Value *alloc = nullptr;

    ~VariableDeclaration() override {
        delete expression;
    }

    llvm::Value *Accept(class Visitor *visitor) override {
        return visitor->Visit(this);
    }

    TypeDefinition *GetCompiledType(Visitor *visitor);
};

class FunctionParameter : public VariableDeclaration {
public:
    using VariableDeclaration::VariableDeclaration;

    int index;
    Function *function;

    llvm::Value *Accept(class Visitor *visitor) override {
        return visitor->Visit(this);
    }
};

class IfStatement : public Node {
public:
    using Node::Node;

    Node *condition = nullptr;
    std::vector<Node *> body;
    IfStatement *elseStatement = nullptr;

    ~IfStatement() override {
        delete condition;
        delete elseStatement;

        for (auto &node: body)
            delete node;
    }

    llvm::Value *Accept(class Visitor *visitor) override {
        return visitor->Visit(this);
    }
};

class ForLoop : public Node {
public:
    using Node::Node;

    Node *value = nullptr; // what to loop over (range, array, etc.)
    Node *step = nullptr; // step [expr]
    VariableDeclaration *identifier = nullptr; // as [name] (defaults to `as it`)
    std::vector<Node *> body;

    ~ForLoop() override {
        delete value;
        delete step;

        for (auto &node: body)
            delete node;
    }

    llvm::Value *Accept(class Visitor *visitor) override {
        return visitor->Visit(this);
    }
};

class Continue : public Node {
public:
    using Node::Node;

    llvm::Value *Accept(class Visitor *visitor) override {
        return visitor->Visit(this);
    }
};

class Break : public Node {
public:
    using Node::Node;

    llvm::Value *Accept(class Visitor *visitor) override {
        return visitor->Visit(this);
    }
};