#pragma once

#include "../Context.h"
#include "../../compiler/Compiler.h"

class FunctionCompilerContext : public Context {
public:
    explicit FunctionCompilerContext(Visitor* visitor, Function* function) : Context(visitor), function(function) {
    }

    TypeVariant* getReturnType() override {
        return function->returnType->ResolveType();
    }

    void handleReturn(const Node* node, llvm::Value* value) override {
        Compiler::getBuilder().CreateRet(value);
    }

public:
    Function* const function;
};
