#pragma once

#include "../Context.h"
#include "../../compiler/Compiler.h"
#include "../../ast/topLevel/Function.h"

class FunctionCompilerContext : public Context {
public:
    explicit FunctionCompilerContext(Visitor* visitor, Function* function) : Context(visitor), function(function) {
    }

    TypeReference* getReturnType() override {
        return function->returnType;
    }

    void handleReturn(Node* node) override {
        Compiler::getBuilder().CreateRetVoid();
    }

    void handleReturn(Node* node, llvm::Value* value) override {
        Compiler::getBuilder().CreateRet(value);
    }

public:
    Function* const function;
};
