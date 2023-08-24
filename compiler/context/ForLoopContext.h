#pragma once

#include "CodegenContext.h"
#include "../../ast/Statements.h"
#include "../Compiler.h"

class ForLoopContext : public CodegenContext {
public:
    explicit ForLoopContext(Visitor *visitor, ForLoop *loop) : CodegenContext(visitor), loop(loop) {}

    void handleReturn(const Node *node, llvm::Value *value) override {
        Compiler::getBuilder().CreateRet(value);
    }

    void handleContinue(const Node *node) override {
        Compiler::getBuilder().CreateBr(tailBlock);
    }

    void handleBreak(const Node *node) override {
        Compiler::getBuilder().CreateBr(exitBlock);
    }

public:
    ForLoop *const loop;
    llvm::BasicBlock *tailBlock;
    llvm::BasicBlock *exitBlock;
};