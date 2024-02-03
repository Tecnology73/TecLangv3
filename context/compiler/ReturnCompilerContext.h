#pragma once

#include "../Context.h"
#include "FunctionCompilerContext.h"

class ReturnCompilerContext : public Context {
public:
    explicit ReturnCompilerContext(Visitor* visitor, Return* aReturn) : Context(visitor), aReturn(aReturn) {
    }

    void handleReturn(Node* node, llvm::Value* value) override {
        Compiler::getBuilder().CreateRet(value);
    }

public:
    Return* const aReturn;
};
