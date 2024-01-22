#pragma once

#include <vector>
#include <unordered_map>
#include <llvm/IR/Function.h>
#include "../Node.h"
#include "../statements/VarDeclaration.h"

class FunctionParameter;

class Function : public Node {
public:
    using Node::Node;

    explicit Function(const std::string_view name) : Node(Token{}), name(name), isExternal(true) {
    }

    Function(const Token& beginToken, const std::string_view name) : Node(beginToken), name(name) {
    }

    void Accept(Visitor* visitor) override {
        visitor->Visit(this);
    }

    bool AddParameter(const Token& token, std::string paramName, TypeReference* type);

    FunctionParameter* GetParameter(const std::string& parameterName) const;

    FunctionParameter* GetParameter(unsigned index) const;

    int GetParameterIndex(const std::string& parameterName);

public:
    std::string_view const name;
    std::unordered_map<std::string, FunctionParameter *> parameters;
    std::vector<std::string> parameterOrder;
    std::vector<Node *> body;
    TypeReference* returnType = nullptr;
    TypeBase* ownerType = nullptr;
    bool isExternal = false;

    llvm::Function* llvmFunction = nullptr;
};

class FunctionParameter : public VariableDeclaration {
public:
    FunctionParameter(
        const Token& beginToken,
        const std::string& name,
        TypeReference* type,
        Function* function
    ) : VariableDeclaration(beginToken, name, type),
        function(function) {
    }

    void Accept(Visitor* visitor) override {
        visitor->Visit(this);
    }

public:
    Function* const function = nullptr;

    int index = 0;
};
