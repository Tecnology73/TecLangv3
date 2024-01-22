#include "Value.h"
#include "Common.h"
#include "../../../compiler/Compiler.h"
#include "../../../compiler/TypeCoercion.h"

llvm::Value* BinaryOperation_Value::generateAdd(llvm::Value* lhs, llvm::Value* rhs) {
    if (lhs->getType()->isDoubleTy())
        return Compiler::getBuilder().CreateFAdd(lhs, rhs, "addtmp");

    return Compiler::getBuilder().CreateAdd(lhs, rhs, "addtmp");
}

llvm::Value* BinaryOperation_Value::generateSub(llvm::Value* lhs, llvm::Value* rhs) {
    if (lhs->getType()->isDoubleTy())
        return Compiler::getBuilder().CreateFSub(lhs, rhs, "subtmp");

    return Compiler::getBuilder().CreateSub(lhs, rhs, "subtmp");
}

llvm::Value* BinaryOperation_Value::generateMultiply(llvm::Value* lhs, llvm::Value* rhs) {
    if (lhs->getType()->isDoubleTy())
        return Compiler::getBuilder().CreateFMul(lhs, rhs, "multmp");

    return Compiler::getBuilder().CreateMul(lhs, rhs, "multmp");
}

llvm::Value* BinaryOperation_Value::generateDivide(llvm::Value* lhs, llvm::Value* rhs) {
    if (lhs->getType()->isDoubleTy())
        return Compiler::getBuilder().CreateFDiv(lhs, rhs, "divtmp");

    return Compiler::getBuilder().CreateSDiv(lhs, rhs, "divtmp");
}

llvm::Value* BinaryOperation_Value::generateLessThan(llvm::Value* lhs, llvm::Value* rhs) {
    if (lhs->getType()->isDoubleTy())
        return Compiler::getBuilder().CreateFCmpOLT(lhs, rhs, "lttmp");

    return Compiler::getBuilder().CreateICmpSLT(lhs, rhs, "lttmp");
}

llvm::Value* BinaryOperation_Value::generateGreaterThan(llvm::Value* lhs, llvm::Value* rhs) {
    if (lhs->getType()->isDoubleTy())
        return Compiler::getBuilder().CreateFCmpOGT(lhs, rhs, "gttmp");

    return Compiler::getBuilder().CreateICmpSGT(lhs, rhs, "gttmp");
}

llvm::Value* BinaryOperation_Value::generateLessThanOrEqual(llvm::Value* lhs, llvm::Value* rhs) {
    if (lhs->getType()->isDoubleTy())
        return Compiler::getBuilder().CreateFCmpOLE(lhs, rhs, "letmp");

    return Compiler::getBuilder().CreateICmpSLE(lhs, rhs, "letmp");
}

llvm::Value* BinaryOperation_Value::generateGreaterThanOrEqual(llvm::Value* lhs, llvm::Value* rhs) {
    if (lhs->getType()->isDoubleTy())
        return Compiler::getBuilder().CreateFCmpOGE(lhs, rhs, "getmp");

    return Compiler::getBuilder().CreateICmpSGE(lhs, rhs, "getmp");
}

llvm::Value* BinaryOperation_Value::generateEqual(llvm::Value* lhs, llvm::Value* rhs) {
    if (lhs->getType()->isDoubleTy())
        return Compiler::getBuilder().CreateFCmpOEQ(lhs, rhs, "eqtmp");

    return Compiler::getBuilder().CreateICmpEQ(lhs, rhs, "eqtmp");
}

llvm::Value* BinaryOperation_Value::generateNotEqual(llvm::Value* lhs, llvm::Value* rhs) {
    if (lhs->getType()->isDoubleTy())
        return Compiler::getBuilder().CreateFCmpONE(lhs, rhs, "netmp");

    return Compiler::getBuilder().CreateICmpNE(lhs, rhs, "netmp");
}

llvm::Value* BinaryOperation_Value::generateLogicalAnd(llvm::Value* lhs, llvm::Value* rhs) {
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
        lhs,
        Compiler::getBuilder().getInt32(0),
        "and_lhs"
    );
    Compiler::getBuilder().CreateCondBr(lhsValue, rhsBlock, mergeBlock);

    // Compare expression.second
    // If it's 1, br and_merge
    // If it's 0, br and_merge
    Compiler::getBuilder().SetInsertPoint(rhsBlock);
    auto rhsValue = Compiler::getBuilder().CreateICmpNE(
        rhs,
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

llvm::Value* BinaryOperation_Value::generateLogicalOr(llvm::Value* lhs, llvm::Value* rhs) {
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
        lhs,
        Compiler::getBuilder().getInt32(0),
        "or_lhs"
    );
    Compiler::getBuilder().CreateCondBr(lhsValue, mergeBlock, rhsBlock);

    // Compare expression.second
    // If it's 1, br or_merge
    // If it's 0, br or_merge
    Compiler::getBuilder().SetInsertPoint(rhsBlock);
    auto rhsValue = Compiler::getBuilder().CreateICmpNE(
        rhs,
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

llvm::Value* BinaryOperation_Value::generateAssign(llvm::Value* lhs, llvm::Value* rhs) {
    return Compiler::getBuilder().CreateStore(rhs, lhs);
}

llvm::Value* BinaryOperation_Value::generatePlusEqual(llvm::Value* lhs, llvm::Value* rhs) {
    llvm::Value* result;
    if (lhs->getType()->isDoubleTy())
        result = Compiler::getBuilder().CreateFAdd(lhs, rhs, "addtmp");
    else
        result = Compiler::getBuilder().CreateAdd(lhs, rhs, "addtmp");

    return Compiler::getBuilder().CreateStore(result, lhs);
}

void BinaryOperation_Value::generate(Visitor* v, const BinaryOperation* node) {
    auto lhs = BinaryOperation_Common::generateValue(v, node, node->lhs);
    auto rhs = BinaryOperation_Common::generateValue(v, node, node->rhs);

    auto commonType = TypeCoercion::getCommonType(lhs->getType(), rhs->getType());
    lhs = TypeCoercion::coerce(lhs, commonType);
    rhs = TypeCoercion::coerce(rhs, commonType);

    llvm::Value* value;

    switch (node->op) {
        case Token::Type::Plus:
            value = generateAdd(lhs, rhs);
            break;
        case Token::Type::Minus:
            value = generateSub(lhs, rhs);
            break;
        case Token::Type::Multiply:
            value = generateMultiply(lhs, rhs);
            break;
        case Token::Type::Divide:
            value = generateDivide(lhs, rhs);
            break;
        case Token::Type::LessThan:
            value = generateLessThan(lhs, rhs);
            break;
        case Token::Type::GreaterThan:
            value = generateGreaterThan(lhs, rhs);
            break;
        case Token::Type::LessThanOrEqual:
            value = generateLessThanOrEqual(lhs, rhs);
            break;
        case Token::Type::GreaterThanOrEqual:
            value = generateGreaterThanOrEqual(lhs, rhs);
            break;
        case Token::Type::LogicalAnd:
            value = generateLogicalAnd(lhs, rhs);
            break;
        case Token::Type::LogicalOr:
            value = generateLogicalOr(lhs, rhs);
            break;
        case Token::Type::PlusEqual:
        case Token::Type::PlusPlus:
            value = generatePlusEqual(lhs, rhs);
            break;
        /*case Token::Type::MinusEqual:
        case Token::Type::MinusMinus:
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
            v->ReportError(ErrorCode::BINARY_OP_UNKNOWN, {std::to_string(static_cast<int>(node->op))}, node);
            return;
    }

    if (value == nullptr) {
        v->ReportError(ErrorCode::UNKNOWN_ERROR, {}, node);
        return;
    }

    v->AddSuccess(value);
}
