#pragma once

#include "../Context.h"
#include "FunctionCompilerContext.h"

class ReturnCompilerContext : public Context {
public:
    explicit ReturnCompilerContext(Visitor* visitor, Return* aReturn) : Context(visitor), aReturn(aReturn) {
    }

    TypeVariant* getReturnType() override {
        if (auto functionContext = dynamic_cast<FunctionCompilerContext *>(parent))
            return functionContext->getReturnType();

        return nullptr;
    }

    void handleReturn(const Node* node, llvm::Value* value) override {
        Compiler::getBuilder().CreateRet(value);
    }

public:
    Return* const aReturn;
};
