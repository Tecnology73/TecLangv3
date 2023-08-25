#pragma once

#include <llvm/IR/Value.h>
#include "../../ast/Expressions.h"
#include "../context/WhenContext.h"

llvm::Value *generateWhen(Visitor *v, When *node) {
    // Setup context
    auto context = Compiler::getScopeManager().enter("when", new WhenContext(v, node));

    // Setup switch
    auto entryBlock = Compiler::getBuilder().GetInsertBlock();
    context->exitBlock = llvm::BasicBlock::Create(Compiler::getContext(), "when.exit", entryBlock->getParent());

    auto matchValue = node->expression->Accept(v);
    auto switchInst = Compiler::getBuilder().CreateSwitch(matchValue, context->exitBlock, node->body.size());

    // Conditions
    for (const auto &item: node->body) {
        auto caseBlock = llvm::BasicBlock::Create(
            Compiler::getContext(),
            item->condition ? "when.case" : "when.else",
            entryBlock->getParent()
        );
        Compiler::getBuilder().SetInsertPoint(caseBlock);

        if (item->condition) {
            auto caseValue = TypeCoercion::coerce(item->condition->Accept(v), matchValue->getType());
            switchInst->addCase(llvm::cast<llvm::ConstantInt>(caseValue), caseBlock);
        } else
            switchInst->setDefaultDest(caseBlock);

        // Body
        llvm::Value *lastValue = nullptr;
        for (const auto &nodeItem: item->body)
            lastValue = nodeItem->Accept(v);

        if (item->implicitReturn && lastValue)
            context->handleReturn(item->body.back(), lastValue);
    }

    // Move the exit block to the end of the statement.
    context->exitBlock->moveAfter(switchInst->getSuccessor(switchInst->getNumSuccessors() - 1));

    // FIXME: This is a hacky solution. We should probably just not generate the exitBlock.
    // If we're in a return statement (i.e. return when ...)
    if (context->isParentReturn()) {
        // We need to ensure that the exitBlock isn't empty because that's invalid IR.
        Compiler::getBuilder().SetInsertPoint(context->exitBlock);
        Compiler::getBuilder().CreateRet(context->getReturnType()->getDefaultValue());
    }

    // Cleanup
    Compiler::getBuilder().SetInsertPoint(context->exitBlock);
    Compiler::getScopeManager().popContext();
    Compiler::getScopeManager().leave("when");
    return nullptr;
}
