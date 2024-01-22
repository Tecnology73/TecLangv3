#include "Null.h"
#include "Common.h"
#include "Value.h"
#include "../../../ast/Literals/Null.h"
#include "../../Compiler.h"
#include "../../TypeCoercion.h"

llvm::Value* BinaryOperation_Null::generateEqual(Visitor* v, const BinaryOperation* node) {
    auto lhs = BinaryOperation_Common::generateValue(v, node, node->lhs);
    if (!lhs) return nullptr;

    // LHS == null
    if (dynamic_cast<Null *>(node->rhs))
        return Compiler::getBuilder().CreateIsNull(lhs);

    // LHS == RHS
    auto rhs = BinaryOperation_Common::generateValue(v, node, node->rhs);
    if (!rhs) return nullptr;

    auto commonType = TypeCoercion::getCommonType(lhs->getType(), rhs->getType());
    lhs = TypeCoercion::coerce(lhs, commonType);
    rhs = TypeCoercion::coerce(rhs, commonType);

    return BinaryOperation_Value::generateEqual(lhs, rhs);
}

llvm::Value* BinaryOperation_Null::generateNotEqual(Visitor* v, const BinaryOperation* node) {
    auto lhs = BinaryOperation_Common::generateValue(v, node, node->lhs);
    if (!lhs) return nullptr;

    // LHS != null
    if (dynamic_cast<Null *>(node->rhs))
        return Compiler::getBuilder().CreateIsNotNull(lhs);

    // LHS != RHS
    auto rhs = BinaryOperation_Common::generateValue(v, node, node->rhs);
    if (!rhs) return nullptr;

    return BinaryOperation_Value::generateNotEqual(lhs, rhs);
}

llvm::Value* BinaryOperation_Null::generateAssign(Visitor* v, const BinaryOperation* node) {
    auto lhs = BinaryOperation_Common::generateValue(v, node, node->lhs);
    if (lhs == nullptr) return nullptr;

    // LHS = null
    if (dynamic_cast<Null *>(node->rhs)) {
        auto rhs = llvm::ConstantPointerNull::get(lhs->getType()->getPointerTo());

        return Compiler::getBuilder().CreateStore(rhs, lhs);
    }

    // LHS = RHS
    auto rhs = BinaryOperation_Common::generateValue(v, node, node->rhs);
    if (rhs == nullptr) return nullptr;

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
