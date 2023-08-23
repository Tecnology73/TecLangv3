#pragma once

#include <llvm/IR/Value.h>
#include "../../ast/Expressions.h"
#include "../codegen/WhenContext.h"

namespace {
    // Check if the When conditions are valid.
    // It's considered valid if:
    // 1. The value is not an enum, or;
    // 2. The value is an enum and all the enum values are covered.
    bool isWhenValid(Visitor *visitor, When *when) {
        // If an else is present, we consider all cases covered.
        if (when->hasElse) return true;

        // Figure out if we're using an Enum for the value.
        auto varRef = dynamic_cast<VariableReference *>(when->condition);
        if (!varRef) return true;

        auto var = Compiler::getScopeManager().getVar(varRef->name);
        if (!var) return true;

        auto anEnum = dynamic_cast<Enum *>(var->type);
        if (!anEnum) return true;

        // Validate all the values are covered.
        std::set<std::string> seenValues;
        for (const auto &item: when->body) {
            // Every item should have a condition (i.e. not be an else)
            // if `when->hasElse` is false.
            auto value = dynamic_cast<StaticRef *>(item->condition);
            if (!value) continue;

            if (value->name != anEnum->name) continue;
            seenValues.emplace(value->next->name);
        }

        when->areAllCasesCovered = seenValues.size() == anEnum->values.size();
        return when->areAllCasesCovered;
    }
}

llvm::Value *generateWhen(Visitor *v, When *node) {
    // Setup context
    auto context = Compiler::getScopeManager().enter("when", new WhenContext(v, node));

    // Validate conditions
    if (!isWhenValid(v, node)) {
        v->ReportError(ErrorCode::WHEN_UNUSED_ENUM_VALUE, {}, node);
        return nullptr;
    }

    // Setup switch
    auto entryBlock = Compiler::getBuilder().GetInsertBlock();
    context->exitBlock = llvm::BasicBlock::Create(Compiler::getContext(), "when.exit", entryBlock->getParent());

    auto matchValue = node->condition->Accept(v);
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
