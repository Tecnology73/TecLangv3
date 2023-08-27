#pragma once

#include <llvm/IR/Value.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Verifier.h>
#include "../../ast/Expressions.h"
#include "../Compiler.h"
#include "../TypeCoercion.h"

namespace {
    llvm::Value *ensureMinBitWidth(llvm::Value *value, int minBitWidth) {
        if (!value->getType()->isIntegerTy())
            return nullptr;

        if (value->getType()->getIntegerBitWidth() >= minBitWidth)
            return value;

        return Compiler::getBuilder().CreateZExt(
            value,
            Compiler::getBuilder().getIntNTy(minBitWidth)
        );
    }

    llvm::Value *ensureDouble(llvm::Value *value) {
        if (value->getType()->isDoubleTy())
            return value;

        if (value->getType()->isIntegerTy())
            return Compiler::getBuilder().CreateSIToFP(value, Compiler::getBuilder().getDoubleTy());

        return nullptr;
    }

    llvm::Value *generateValue(Visitor *v, Node *node) {
        auto value = node->Accept(v);
        if (value == nullptr) return nullptr;

        // We'll just make everything i32 for now.
        if (value->getType()->isIntegerTy())
            value = ensureMinBitWidth(value, 32);

        return value;
    }

    llvm::Value *generateAdd(Visitor *v, const std::pair<llvm::Value *, llvm::Value *> &values) {
        if (values.first->getType()->isDoubleTy())
            return Compiler::getBuilder().CreateFAdd(values.first, values.second, "addtmp");

        return Compiler::getBuilder().CreateAdd(values.first, values.second, "addtmp");
    }

    llvm::Value *generateSub(Visitor *v, const std::pair<llvm::Value *, llvm::Value *> &values) {
        if (values.first->getType()->isDoubleTy())
            return Compiler::getBuilder().CreateFSub(values.first, values.second, "subtmp");

        return Compiler::getBuilder().CreateSub(values.first, values.second, "subtmp");
    }

    llvm::Value *generateMultiply(Visitor *v, const std::pair<llvm::Value *, llvm::Value *> &values) {
        if (values.first->getType()->isDoubleTy())
            return Compiler::getBuilder().CreateFMul(values.first, values.second, "multmp");

        return Compiler::getBuilder().CreateMul(values.first, values.second, "multmp");
    }

    llvm::Value *generateDivide(Visitor *v, const std::pair<llvm::Value *, llvm::Value *> &values) {
        if (values.first->getType()->isDoubleTy())
            return Compiler::getBuilder().CreateFDiv(values.first, values.second, "divtmp");

        return Compiler::getBuilder().CreateSDiv(values.first, values.second, "divtmp");
    }

    llvm::Value *generateLessThan(Visitor *v, const std::pair<llvm::Value *, llvm::Value *> &values) {
        if (values.first->getType()->isDoubleTy())
            return Compiler::getBuilder().CreateFCmpOLT(values.first, values.second, "lttmp");

        return Compiler::getBuilder().CreateICmpSLT(values.first, values.second, "lttmp");
    }

    llvm::Value *generateGreaterThan(Visitor *v, const std::pair<llvm::Value *, llvm::Value *> &values) {
        if (values.first->getType()->isDoubleTy())
            return Compiler::getBuilder().CreateFCmpOGT(values.first, values.second, "gttmp");

        return Compiler::getBuilder().CreateICmpSGT(values.first, values.second, "gttmp");
    }

    llvm::Value *generateLessThanOrEqual(Visitor *v, const std::pair<llvm::Value *, llvm::Value *> &values) {
        if (values.first->getType()->isDoubleTy())
            return Compiler::getBuilder().CreateFCmpOLE(values.first, values.second, "letmp");

        return Compiler::getBuilder().CreateICmpSLE(values.first, values.second, "letmp");
    }

    llvm::Value *generateGreaterThanOrEqual(Visitor *v, const std::pair<llvm::Value *, llvm::Value *> &values) {
        if (values.first->getType()->isDoubleTy())
            return Compiler::getBuilder().CreateFCmpOGE(values.first, values.second, "getmp");

        return Compiler::getBuilder().CreateICmpSGE(values.first, values.second, "getmp");
    }

    llvm::Value *generateEqual(Visitor *v, const std::pair<llvm::Value *, llvm::Value *> &values) {
        if (values.first->getType()->isDoubleTy())
            return Compiler::getBuilder().CreateFCmpOEQ(values.first, values.second, "eqtmp");

        return Compiler::getBuilder().CreateICmpEQ(values.first, values.second, "eqtmp");
    }

    llvm::Value *generateNotEqual(Visitor *v, const std::pair<llvm::Value *, llvm::Value *> &values) {
        if (values.first->getType()->isDoubleTy())
            return Compiler::getBuilder().CreateFCmpONE(values.first, values.second, "netmp");

        return Compiler::getBuilder().CreateICmpNE(values.first, values.second, "netmp");
    }

    llvm::Value *generateLogicalAnd(Visitor *visitor, const std::pair<llvm::Value *, llvm::Value *> &values) {
        auto lhsBlock = llvm::BasicBlock::Create(
            Compiler::getContext(),
            "and.lhs",
            Compiler::getBuilder().GetInsertBlock()->getParent()
        );
        auto rhsBlock = llvm::BasicBlock::Create(
            Compiler::getContext(),
            "and.rhs",
            Compiler::getBuilder().GetInsertBlock()->getParent()
        );
        auto mergeBlock = llvm::BasicBlock::Create(
            Compiler::getContext(),
            "and.merge",
            Compiler::getBuilder().GetInsertBlock()->getParent()
        );

        // Compare values.first
        // If it's 1, compare values.second
        // If it's 0, br and_merge
        Compiler::getBuilder().CreateBr(lhsBlock);
        Compiler::getBuilder().SetInsertPoint(lhsBlock);
        auto lhsValue = Compiler::getBuilder().CreateICmpNE(
            values.first,
            Compiler::getBuilder().getInt32(0),
            "and_lhs"
        );
        Compiler::getBuilder().CreateCondBr(lhsValue, rhsBlock, mergeBlock);

        // Compare expression.second
        // If it's 1, br and_merge
        // If it's 0, br and_merge
        Compiler::getBuilder().SetInsertPoint(rhsBlock);
        auto rhsValue = Compiler::getBuilder().CreateICmpNE(
            values.second,
            Compiler::getBuilder().getInt32(0),
            "and_rhs"
        );
        Compiler::getBuilder().CreateBr(mergeBlock);

        // MergeBlock - This is where we'll determine the result of the logical and.
        Compiler::getBuilder().SetInsertPoint(mergeBlock);
        auto phi = Compiler::getBuilder().CreatePHI(
            Compiler::getBuilder().getInt1Ty(),
            2,
            "and_result"
        );

        // If we came from the lhs block, we know it's false.
        // If we came from the rhs block, use the result of the `rhsValue`.
        phi->addIncoming(Compiler::getBuilder().getInt1(false), lhsBlock);
        phi->addIncoming(rhsValue, rhsBlock);

        return phi;
    }

    llvm::Value *generateLogicalOr(Visitor *v, const std::pair<llvm::Value *, llvm::Value *> &values) {
        auto lhsBlock = llvm::BasicBlock::Create(
            Compiler::getContext(),
            "or.lhs",
            Compiler::getBuilder().GetInsertBlock()->getParent()
        );
        auto rhsBlock = llvm::BasicBlock::Create(
            Compiler::getContext(),
            "or.rhs",
            Compiler::getBuilder().GetInsertBlock()->getParent()
        );
        auto mergeBlock = llvm::BasicBlock::Create(
            Compiler::getContext(),
            "or.merge",
            Compiler::getBuilder().GetInsertBlock()->getParent()
        );

        // Compare values.first
        // If it's 1, br or_merge
        // If it's 0, compare values.second
        Compiler::getBuilder().CreateBr(lhsBlock);
        Compiler::getBuilder().SetInsertPoint(lhsBlock);
        auto lhsValue = Compiler::getBuilder().CreateICmpNE(
            values.first,
            Compiler::getBuilder().getInt32(0),
            "or_lhs"
        );
        Compiler::getBuilder().CreateCondBr(lhsValue, mergeBlock, rhsBlock);

        // Compare expression.second
        // If it's 1, br or_merge
        // If it's 0, br or_merge
        Compiler::getBuilder().SetInsertPoint(rhsBlock);
        auto rhsValue = Compiler::getBuilder().CreateICmpNE(
            values.second,
            Compiler::getBuilder().getInt32(0),
            "or_rhs"
        );
        Compiler::getBuilder().CreateBr(mergeBlock);

        // MergeBlock - This is where we'll determine the result of the logical and.
        Compiler::getBuilder().SetInsertPoint(mergeBlock);
        auto phi = Compiler::getBuilder().CreatePHI(
            Compiler::getBuilder().getInt1Ty(),
            2,
            "or_result"
        );

        // If we came from the lhs block, we know it's true.
        // If we came from the rhs block, use the result of the `rhsValue`.
        phi->addIncoming(Compiler::getBuilder().getInt1(true), lhsBlock);
        phi->addIncoming(rhsValue, rhsBlock);

        return phi;
    }

    llvm::Value *generateAssign(Visitor *v, const std::pair<llvm::Value *, llvm::Value *> &values) {
        return Compiler::getBuilder().CreateStore(values.second, values.first);
    }

    llvm::Value *generatePlusEqual(Visitor *v, const std::pair<llvm::Value *, llvm::Value *> &values) {
        if (values.first->getType()->isDoubleTy())
            return Compiler::getBuilder().CreateFAdd(values.first, values.second, "addtmp");

        return Compiler::getBuilder().CreateAdd(values.first, values.second, "addtmp");
    }
}

llvm::Value *generateBinaryOperation(Visitor *visitor, BinaryOperation *node) {
    auto lhs = generateValue(visitor, node->lhs);
    auto rhs = generateValue(visitor, node->rhs);
    std::pair<llvm::Value *, llvm::Value *> values;
    if (node->op == Token::Type::Divide) {
        // If we're doing a division, we need to make sure that both sides are doubles.
        values.first = TypeCoercion::coerce(lhs, Compiler::getScopeManager().getType("double")->llvmType);
        values.second = TypeCoercion::coerce(rhs, Compiler::getScopeManager().getType("double")->llvmType);
    } else
        values = TypeCoercion::coerce(lhs, rhs);

    switch (node->op) {
        case Token::Type::Plus:
            return generateAdd(visitor, values);
        case Token::Type::Minus:
            return generateSub(visitor, values);
        case Token::Type::Multiply:
            return generateMultiply(visitor, values);
        case Token::Type::Divide:
            return generateDivide(visitor, values);
        case Token::Type::LessThan:
            return generateLessThan(visitor, values);
        case Token::Type::GreaterThan:
            return generateGreaterThan(visitor, values);
        case Token::Type::LessThanOrEqual:
            return generateLessThanOrEqual(visitor, values);
        case Token::Type::GreaterThanOrEqual:
            return generateGreaterThanOrEqual(visitor, values);
        case Token::Type::Equal:
            return generateEqual(visitor, values);
        case Token::Type::NotEqual:
            return generateNotEqual(visitor, values);
        case Token::Type::LogicalAnd:
            return generateLogicalAnd(visitor, values);
        case Token::Type::LogicalOr:
            return generateLogicalOr(visitor, values);
        case Token::Type::Assign:
            return generateAssign(visitor, values);
        case Token::Type::PlusEqual:
        case Token::Type::PlusPlus:
            return generatePlusEqual(visitor, values);
            /*case Token::Type::MinusEqual:
                break;
            case Token::Type::MultiplyEqual:
                break;
            case Token::Type::DivideEqual:
                break;
            case Token::Type::PercentEqual:
                break;
            case Token::Type::AmpersandEqual:
                break;
            case Token::Type::PipeEqual:
                break;*/
        default:
            visitor->ReportError(ErrorCode::BINARY_OP_UNKNOWN, {std::to_string(static_cast<int>(node->op))}, node);
            return nullptr;
    }
}
