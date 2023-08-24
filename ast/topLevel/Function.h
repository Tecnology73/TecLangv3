#pragma once

#include <vector>
#include <map>
#include "../Node.h"
#include "TypeBase.h"
#include "../statements/VarDeclaration.h"

class Function;

class FunctionParameter : public Node {
public:
    FunctionParameter(Token beginToken, std::string name, TypeBase *type, Function *function) : Node(std::move(beginToken)), name(std::move(name)), type(type), function(function) {}

    llvm::Value *Accept(Visitor *visitor) override {
        return visitor->Visit(this);
    }

public:
    std::string const name;
    Node *const value = nullptr;
    TypeBase *const type = nullptr;
    Function *const function = nullptr;

    int index;
};

class Function : public Node {
public:
    using Node::Node;

    Function(std::string name) : Node(Token{}), name(std::move(name)), isExternal(true) {}

    Function(Token beginToken) : Node(std::move(beginToken)), name(token.value) {}

    llvm::Value *Accept(Visitor *visitor) override {
        return visitor->Visit(this);
    }

    bool addParameter(Token token, std::string paramName, TypeBase *type);

    FunctionParameter *getParameter(const std::string &parameterName) const;

    int getParameterIndex(const std::string &parameterName);

public:
    std::string const name;
    std::vector<FunctionParameter *> parameters;
    std::map<std::string, int> parameterIndices;
    std::vector<Node *> body;
    TypeBase *returnType = nullptr;
    TypeBase *ownerType = nullptr;
    bool isExternal = false;

    llvm::Function *llvmFunction = nullptr;
};