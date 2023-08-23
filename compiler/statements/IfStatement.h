#pragma once

#include <llvm/IR/Value.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Verifier.h>
#include "../../ast/Statements.h"
#include "../Compiler.h"

llvm::Value *generateIfStatement(Visitor *v, IfStatement *node);

namespace {
    llvm::Value *generateElseStatement(Visitor *v, IfStatement *node) {
        if (node->condition) return generateIfStatement(v, node);

        for (const auto &item: node->body)
            item->Accept(v);

        auto exitBlock = llvm::BasicBlock::Create(
            Compiler::getContext(),
            "if.exit",
            Compiler::getBuilder().GetInsertBlock()->getParent()
        );
        Compiler::getBuilder().CreateBr(exitBlock);
        Compiler::getBuilder().SetInsertPoint(exitBlock);

        return nullptr;
    }
}

llvm::Value *generateIfStatement(Visitor *v, IfStatement *node) {
    llvm::Value *condition = nullptr;
    if (node->condition) {
        condition = Compiler::getBuilder().CreateICmpNE(
            node->condition->Accept(v),
            Compiler::getBuilder().getInt1(false),
            "@if.cond"
        );
    }

    auto thenBlock = llvm::BasicBlock::Create(
        Compiler::getContext(),
        "if.then",
        Compiler::getBuilder().GetInsertBlock()->getParent()
    );
    auto exitBlock = llvm::BasicBlock::Create(
        Compiler::getContext(),
        "if.else",
        Compiler::getBuilder().GetInsertBlock()->getParent()
    );

    Compiler::getBuilder().CreateCondBr(condition, thenBlock, exitBlock);

    // Then block
    Compiler::getBuilder().SetInsertPoint(thenBlock);
    for (const auto &item: node->body)
        item->Accept(v);

    // Else block
    Compiler::getBuilder().SetInsertPoint(exitBlock);
    if (node->elseStatement)
        generateElseStatement(v, node->elseStatement);

    // If there is no return statement in the then block, we need to create a branch to the exit block.
    if (!thenBlock->getTerminator()) {
        auto currentBlock = Compiler::getBuilder().GetInsertBlock();

        Compiler::getBuilder().SetInsertPoint(thenBlock, thenBlock->end());
        Compiler::getBuilder().CreateBr(currentBlock);
        Compiler::getBuilder().SetInsertPoint(currentBlock);
    }

    return nullptr;
}