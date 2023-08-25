#pragma once

#include <llvm/IR/Value.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Verifier.h>
#include "../../ast/Statements.h"
#include "../Compiler.h"
#include "../context/ForLoopContext.h"

llvm::Value *generateForLoop(Visitor *v, ForLoop *node) {
    // Setup context
    auto context = Compiler::getScopeManager().enter("for", new ForLoopContext(v, node));

    // Basic Blocks
    auto entryBlock = Compiler::getBuilder().GetInsertBlock();
    auto loop = llvm::BasicBlock::Create(Compiler::getContext(), "loop", entryBlock->getParent());
    auto loopBody = llvm::BasicBlock::Create(Compiler::getContext(), "loop.body", entryBlock->getParent());
    auto loopTail = context->tailBlock = llvm::BasicBlock::Create(Compiler::getContext(), "loop.tail", entryBlock->getParent());
    auto loopExit = context->exitBlock = llvm::BasicBlock::Create(Compiler::getContext(), "loop.exit", entryBlock->getParent());

    //
    // Head - Create iterator & check loop expression
    //
    Compiler::getBuilder().CreateBr(loop);
    Compiler::getBuilder().SetInsertPoint(loop);

    // TODO: Support more than just a range expression (lower..upper)
    auto cond = (BinaryOperation *) node->value;

    // iterator
    auto it = new VariableDeclaration(node->token);
    // it->type = inferType(v, cond->lhs);
    it->type = Compiler::getScopeManager().getType("i32");

    auto keyPhi = Compiler::getBuilder().CreatePHI(it->type->llvmType, 2, node->identifier->name);
    it->alloc = keyPhi;

    Compiler::getScopeManager().add(it);

    // Initial expression of the iterator
    keyPhi->addIncoming(
        TypeCoercion::coerce(cond->lhs->Accept(v), it->type->llvmType), // Range lower bound
        entryBlock
    );

    // Loop expression
    auto conditionValue = Compiler::getBuilder().CreateICmpSLT(
        keyPhi,
        TypeCoercion::coerce(cond->rhs->Accept(v), it->type->llvmType), // Range upper bound
        "cond"
    );
    Compiler::getBuilder().CreateCondBr(conditionValue, loopBody, loopExit);

    //
    // Body
    //
    Compiler::getBuilder().SetInsertPoint(loopBody);
    for (const auto &item: node->body)
        item->Accept(v);

    //
    // Tail - Increment iterator & jump back to head
    //
    Compiler::getBuilder().CreateBr(loopTail);
    Compiler::getBuilder().SetInsertPoint(loopTail);

    llvm::Value *stepValue;
    if (node->step)
        stepValue = TypeCoercion::coerce(node->step->Accept(v), it->type->llvmType);
    else
        stepValue = Compiler::getBuilder().getInt32(1);

    auto nextIndex = Compiler::getBuilder().CreateAdd(keyPhi, stepValue, "next_index");
    keyPhi->addIncoming(nextIndex, Compiler::getBuilder().GetInsertBlock());

    Compiler::getBuilder().CreateBr(loop);

    // Move the exit block to the end of the statement.
    loopExit->moveAfter(&entryBlock->getParent()->back());

    // Cleanup
    Compiler::getScopeManager().popContext();
    Compiler::getScopeManager().leave("for");
    Compiler::getBuilder().SetInsertPoint(loopExit);

    return nullptr;
}
