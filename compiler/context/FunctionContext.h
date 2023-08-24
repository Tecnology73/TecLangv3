#pragma once

#include "CodegenContext.h"
#include "../../ast/Statements.h"
#include "../Compiler.h"

class FunctionContext : public CodegenContext {
public:
    explicit FunctionContext(Visitor *visitor, Function *function) : CodegenContext(visitor), function(function) {}

    TypeDefinition *getReturnType() override {
        // return function->returnType;
        return nullptr;
    }

    void handleReturn(const Node *node, llvm::Value *value) override {
        Compiler::getBuilder().CreateRet(value);
        addReturnType(node, value);
    }

    void addReturnType(const Node *node, llvm::Value *value) {
        if (returnStatements.find(value->getType()) == returnStatements.end())
            returnStatements.emplace(value->getType(), std::vector<const Return *>());

        returnStatements[value->getType()].emplace_back(dynamic_cast<const Return *>(node));
    }

public:
    Function *const function;
    // Keep track of return statements, so we can infer the return type if required.
    std::map<llvm::Type *, std::vector<const Return *>> returnStatements;
};