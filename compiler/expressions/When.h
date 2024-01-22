#pragma once

#include <llvm/IR/Value.h>
#include "../../ast/Expressions.h"
#include "../../context/compiler/WhenCompilerContext.h"

void generateWhen(Visitor* v, When* node) {
    // Setup context
    auto context = Compiler::getScopeManager().enter("when", new WhenCompilerContext(v, node));

    // Setup switch
    auto entryBlock = Compiler::getBuilder().GetInsertBlock();
    context->exitBlock = llvm::BasicBlock::Create(Compiler::getContext(), "when.exit", entryBlock->getParent());

    node->expression->Accept(v);

    VisitorResult matchResult;
    if (!v->TryGetResult(matchResult)) return;

    auto matchValue = matchResult.value;
    if (matchValue->getType()->isPointerTy())
        matchValue = Compiler::getBuilder().CreateLoad(matchResult.type->type->getLlvmType(), matchValue);

    auto switchInst = Compiler::getBuilder().CreateSwitch(
        matchValue,
        context->exitBlock,
        node->body.size()
    );

    // Conditions
    for (const auto& item: node->body) {
        auto caseBlock = llvm::BasicBlock::Create(
            Compiler::getContext(),
            item->condition ? "when.case" : "when.else",
            entryBlock->getParent()
        );
        Compiler::getBuilder().SetInsertPoint(caseBlock);

        if (item->condition) {
            item->condition->Accept(v);

            VisitorResult result;
            if (!v->TryGetResult(result)) return;

            auto caseValue = TypeCoercion::coerce(result.value, matchValue->getType());
            switchInst->addCase(llvm::cast<llvm::ConstantInt>(caseValue), caseBlock);
        } else
            switchInst->setDefaultDest(caseBlock);

        // Body
        llvm::Value* lastValue = nullptr;
        for (const auto& nodeItem: item->body) {
            nodeItem->Accept(v);

            VisitorResult result;
            if (!v->TryGetResult(result)) return;

            lastValue = result.value;
        }

        if (item->implicitReturn && lastValue)
            context->handleReturn(item->body.back(), lastValue);
    }

    if (auto defaultDest = switchInst->getDefaultDest()) {
        // Move the else block to the end of the statement.
        // This just makes the IR nicer.
        defaultDest->moveAfter(switchInst->getSuccessor(switchInst->getNumSuccessors() - 1));

        // Move the exit block to after the cases & default block.
        context->exitBlock->moveAfter(defaultDest);
    } else {
        // Move the exit block to after the cases.
        context->exitBlock->moveAfter(switchInst->getSuccessor(switchInst->getNumSuccessors() - 1));
    }

    if (!context->exitBlock->hasNPredecessorsOrMore(1))
        context->exitBlock->eraseFromParent();
    else if (context->isParentReturn()) {
        Compiler::getBuilder().SetInsertPoint(context->exitBlock);

        // We need to ensure that the exit block has something inside of it,
        // otherwise it's considered invalid.
        // FIXME: When all possible cases are covered (such as with an enum), it would be better if we didn't generate an exit block at all.
        Compiler::getBuilder().CreateRet(context->getReturnType()->type->getDefaultValue());
    } else
        Compiler::getBuilder().SetInsertPoint(context->exitBlock);

    // Cleanup
    Compiler::getScopeManager().popContext();
    Compiler::getScopeManager().leave("when");

    v->AddSuccess();
}
