#pragma once

#include <vector>
#include <unordered_map>
#include "../Node.h"
#include "TypeBase.h"
#include "../statements/VarDeclaration.h"

class FunctionParameter;

class Function : public Node {
public:
    using Node::Node;

    Function(std::string name) : Node(Token{}), name(name), isExternal(true) {}

    Function(const Token &beginToken, const std::string &name) : Node(beginToken), name(name) {}

    llvm::Value *Accept(Visitor *visitor) override {
        return visitor->Visit(this);
    }

    bool AddParameter(const Token &token, std::string paramName, TypeBase *type);

    FunctionParameter *GetParameter(const std::string &parameterName) const;

    FunctionParameter *GetParameter(unsigned index) const;

    int GetParameterIndex(const std::string &parameterName);

public:
    std::string const name;
    std::unordered_map<std::string, FunctionParameter *> parameters;
    std::vector<std::string> parameterOrder;
    std::vector<Node *> body;
    TypeBase *returnType = nullptr;
    TypeBase *ownerType = nullptr;
    bool isExternal = false;

    llvm::Function *llvmFunction = nullptr;
};

class FunctionParameter : public VariableDeclaration {
public:
    FunctionParameter(const Token &beginToken, const std::string &name, TypeBase *type, Function *function) :
        VariableDeclaration(beginToken),
        name(name),
        type(type),
        function(function) {}

    llvm::Value *Accept(Visitor *visitor) override {
        return visitor->Visit(this);
    }

public:
    std::string const name;
    TypeBase *const type = nullptr;
    Function *const function = nullptr;

    int index;
};
