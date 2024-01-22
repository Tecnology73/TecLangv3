#pragma once

#include "../Context.h"
#include "../../compiler/Compiler.h"

class ForLoopCompilerContext : public Context {
public:
    explicit ForLoopCompilerContext(Visitor* visitor, ForLoop* loop): Context(visitor), loop(loop) {
    }

    TypeVariant* getReturnType() override {
        if (!parent)
            return nullptr;

        return parent->getReturnType();
    }

    void handleReturn(const Node* node, llvm::Value* value) override {
        Compiler::getBuilder().CreateRet(value);
    }

    void handleContinue(const Node* node) override {
        Compiler::getBuilder().CreateBr(tailBlock);
    }

    void handleBreak(const Node* node) override {
        Compiler::getBuilder().CreateBr(exitBlock);
    }

public:
    ForLoop* const loop;
    llvm::BasicBlock* tailBlock = nullptr;
    llvm::BasicBlock* exitBlock = nullptr;
};
