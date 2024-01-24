#include "Null.h"
#include "Common.h"
#include "Value.h"
#include "../../../ast/literals/Null.h"
#include "../../../ast/literals/String.h"
#include "../../Compiler.h"
#include "../../TypeCoercion.h"

llvm::Value* BinaryOperation_Null::generateEqual(Visitor* v, const BinaryOperation* node) {
    auto lhs = BinaryOperation_Common::generateValue(v, node, node->lhs);

    VisitorResult lhsResult;
    v->TryGetResult(lhsResult);
    if (lhs == nullptr) return nullptr;

    // LHS == null
    if (dynamic_cast<Null *>(node->rhs))
        return Compiler::getBuilder().CreateIsNull(lhs);

    // LHS == RHS
    auto rhs = BinaryOperation_Common::generateValue(v, node, node->rhs, true);

    VisitorResult rhsResult;
    v->TryGetResult(rhsResult);
    if (rhs == nullptr) return nullptr;

    rhs = TypeCoercion::coerce(rhs, lhsResult.type->type->getLlvmType());

    return BinaryOperation_Value::generateEqual(lhs, rhs);
}

llvm::Value* BinaryOperation_Null::generateNotEqual(Visitor* v, const BinaryOperation* node) {
    auto lhs = BinaryOperation_Common::generateValue(v, node, node->lhs);

    VisitorResult lhsResult;
    v->TryGetResult(lhsResult);
    if (lhs == nullptr) return nullptr;

    // LHS != null
    if (dynamic_cast<Null *>(node->rhs))
        return Compiler::getBuilder().CreateIsNotNull(lhs);

    // LHS != RHS
    auto rhs = BinaryOperation_Common::generateValue(v, node, node->rhs, true);

    VisitorResult rhsResult;
    v->TryGetResult(rhsResult);
    if (rhs == nullptr) return nullptr;

    rhs = TypeCoercion::coerce(rhs, lhsResult.type->type->getLlvmType());

    return BinaryOperation_Value::generateNotEqual(lhs, rhs);
}

llvm::Value* BinaryOperation_Null::generateAssign(Visitor* v, const BinaryOperation* node) {
    auto lhs = BinaryOperation_Common::generateValue(v, node, node->lhs);

    VisitorResult lhsResult;
    v->TryGetResult(lhsResult);
    if (lhs == nullptr)
        return nullptr;

    // LHS = null
    if (dynamic_cast<Null *>(node->rhs)) {
        auto rhs = llvm::ConstantPointerNull::get(lhs->getType()->getPointerTo());

        return Compiler::getBuilder().CreateStore(rhs, lhs);
    }

    // LHS = RHS
    auto rhs = BinaryOperation_Common::generateValue(v, node, node->rhs, true);

    VisitorResult rhsResult;
    v->TryGetResult(rhsResult);
    if (rhs == nullptr)
        return nullptr;

    rhs = TypeCoercion::coerce(rhs, lhsResult.type->type->getLlvmType());

    if (lhsResult.type->type->name == "string") {
        // String literals are stored as a global.
        // Inside a function, we normally load the value if it's a pointer (which it is in the case of a string).
        // This is a bit of a hack to get around that.
        if (dynamic_cast<String *>(node->rhs)) {
            rhs = Compiler::getBuilder().CreateAlloca(rhsResult.value->getType());
            Compiler::getBuilder().CreateStore(rhsResult.value, rhs);
        }

        return Compiler::getBuilder().CreateCall(
            lhsResult.type->type->GetFunction("assign")->llvmFunction,
            {lhs, rhs}
        );
    }

    return BinaryOperation_Value::generateAssign(lhs, rhs);
}

llvm::Value* BinaryOperation_Null::generateIs(Visitor* v, const BinaryOperation* node) {
    // obj is null
    if (dynamic_cast<Null *>(node->rhs)) {
        auto lhs = BinaryOperation_Common::generateValue(v, node, node->lhs);
        if (lhs == nullptr) return nullptr;

        return Compiler::getBuilder().CreateIsNull(lhs);
    }

    // (varRef | funcCall | staticRef) is type
    if (auto cNode = dynamic_cast<ChainableNode *>(node->lhs)) {
        auto lhsType = cNode->getFinalType();
        auto rhsType = dynamic_cast<StaticRef *>(node->rhs)->getFinalType();

        return Compiler::getBuilder().getInt1(
            TypeCoercion::isTypeCompatible(lhsType, rhsType)
        );
    }

    // (int | double | bool | string) is type
    if (auto literal = dynamic_cast<Literal *>(node->lhs)) {
        auto lhsType = literal->getType();
        auto rhsType = dynamic_cast<StaticRef *>(node->rhs)->getFinalType();

        return Compiler::getBuilder().getInt1(
            TypeCoercion::isTypeCompatible(lhsType, rhsType)
        );
    }

    return nullptr;
}
