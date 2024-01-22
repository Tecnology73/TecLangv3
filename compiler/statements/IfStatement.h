#pragma once

#include <llvm/IR/Value.h>
#include <llvm/IR/Function.h>
#include "../../ast/Statements.h"
#include "../Compiler.h"

void generateIfStatement(Visitor* v, IfStatement* node);

namespace {
    void generateElseStatement(Visitor* v, IfStatement* node) {
        if (node->condition) {
            generateIfStatement(v, node);
            return;
        }

        for (const auto& item: node->body) {
            item->Accept(v);

            if (VisitorResult result; !v->TryGetResult(result)) return;
        }

        auto exitBlock = llvm::BasicBlock::Create(
            Compiler::getContext(),
            "if.exit",
            Compiler::getBuilder().GetInsertBlock()->getParent()
        );
        Compiler::getBuilder().CreateBr(exitBlock);
        Compiler::getBuilder().SetInsertPoint(exitBlock);

        v->AddSuccess();
    }
}

void generateIfStatement(Visitor* v, IfStatement* node) {
    llvm::Value* condition = nullptr;
    if (node->condition) {
        node->condition->Accept(v);

        VisitorResult result;
        if (!v->TryGetResult(result)) return;

        condition = Compiler::getBuilder().CreateICmpEQ(
            result.value,
            Compiler::getBuilder().getInt1(true),
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
    for (const auto& item: node->body) {
        item->Accept(v);

        if (VisitorResult result; !v->TryGetResult(result)) return;
    }

    // Else block
    Compiler::getBuilder().SetInsertPoint(exitBlock);
    if (node->elseStatement) {
        generateElseStatement(v, node->elseStatement);

        if (VisitorResult result; !v->TryGetResult(result)) return;
    }

    // If there is no return statement in the then block, we need to create a branch to the exit block.
    if (!thenBlock->getTerminator()) {
        auto currentBlock = Compiler::getBuilder().GetInsertBlock();

        Compiler::getBuilder().SetInsertPoint(thenBlock, thenBlock->end());
        Compiler::getBuilder().CreateBr(currentBlock);
        Compiler::getBuilder().SetInsertPoint(currentBlock);
    }

    v->AddSuccess();
}
