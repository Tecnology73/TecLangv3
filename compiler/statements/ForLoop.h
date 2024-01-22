#pragma once

#include <llvm/IR/Value.h>
#include <llvm/IR/Function.h>
#include "../../ast/Statements.h"
#include "../Compiler.h"
#include "../../symbolTable/SymbolTable.h"
#include "../../context/compiler/ForLoopCompilerContext.h"

void generateForLoop(Visitor* v, ForLoop* node) {
    // Setup context
    auto context = Compiler::getScopeManager().enter("for", new ForLoopCompilerContext(v, node));

    // Basic Blocks
    auto entryBlock = Compiler::getBuilder().GetInsertBlock();
    auto loop = llvm::BasicBlock::Create(Compiler::getContext(), "loop", entryBlock->getParent());
    auto loopBody = llvm::BasicBlock::Create(Compiler::getContext(), "loop.body", entryBlock->getParent());
    auto loopTail = context->tailBlock = llvm::BasicBlock::Create(
                        Compiler::getContext(),
                        "loop.tail",
                        entryBlock->getParent()
                    );
    auto loopExit = context->exitBlock = llvm::BasicBlock::Create(
                        Compiler::getContext(),
                        "loop.exit",
                        entryBlock->getParent()
                    );

    //
    // Head - Create iterator & check loop expression
    //
    Compiler::getBuilder().CreateBr(loop);
    Compiler::getBuilder().SetInsertPoint(loop);

    // TODO: Support more than just a range expression (lower..upper)
    auto cond = static_cast<BinaryOperation *>(node->value);

    // iterator
    auto it = node->identifier = new VariableDeclaration(node->token, StringInternTable::Intern("it"));
    // it->type = inferType(v, cond->lhs);
    it->type = std::get<TypeDefinition*>(SymbolTable::GetInstance()->Get("i32")->value)->createVariant()->CreateReference();

    auto keyPhi = Compiler::getBuilder().CreatePHI(it->type->ResolveType()->type->llvmType, 2, node->identifier->name);
    it->alloc = keyPhi;

    Compiler::getScopeManager().Add(it);

    // Initial expression of the iterator
    cond->lhs->Accept(v);
    VisitorResult lhsResult;
    if (!v->TryGetResult(lhsResult)) return;

    keyPhi->addIncoming(
        TypeCoercion::coerce(lhsResult.value, it->type->ResolveType()->type->llvmType),
        // Range lower bound
        entryBlock
    );

    // Loop expression
    cond->rhs->Accept(v);
    VisitorResult rhsResult;
    if (!v->TryGetResult(rhsResult)) return;

    auto conditionValue = Compiler::getBuilder().CreateICmpSLT(
        keyPhi,
        TypeCoercion::coerce(rhsResult.value, it->type->ResolveType()->type->llvmType),
        // Range upper bound
        "cond"
    );
    Compiler::getBuilder().CreateCondBr(conditionValue, loopBody, loopExit);

    //
    // Body
    //
    Compiler::getBuilder().SetInsertPoint(loopBody);
    for (const auto& item: node->body) {
        item->Accept(v);

        if (VisitorResult result; !v->TryGetResult(result)) return;
    }

    //
    // Tail - Increment iterator & jump back to head
    //
    Compiler::getBuilder().CreateBr(loopTail);
    Compiler::getBuilder().SetInsertPoint(loopTail);

    llvm::Value* stepValue;
    if (node->step) {
        node->step->Accept(v);
        VisitorResult stepResult;
        if (!v->TryGetResult(stepResult)) return;

        stepValue = TypeCoercion::coerce(stepResult.value, it->type->ResolveType()->type->llvmType);
    } else
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

    v->AddSuccess();
}
