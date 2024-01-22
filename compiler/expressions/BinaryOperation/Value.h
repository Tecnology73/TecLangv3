#pragma once

#include <llvm/IR/Value.h>
#include "../../../ast/Expressions.h"

namespace BinaryOperation_Value {
    llvm::Value* generateAdd(llvm::Value* lhs, llvm::Value* rhs);

    llvm::Value* generateSub(llvm::Value* lhs, llvm::Value* rhs);

    llvm::Value* generateMultiply(llvm::Value* lhs, llvm::Value* rhs);

    llvm::Value* generateDivide(llvm::Value* lhs, llvm::Value* rhs);

    llvm::Value* generateLessThan(llvm::Value* lhs, llvm::Value* rhs);

    llvm::Value* generateGreaterThan(llvm::Value* lhs, llvm::Value* rhs);

    llvm::Value* generateLessThanOrEqual(llvm::Value* lhs, llvm::Value* rhs);

    llvm::Value* generateGreaterThanOrEqual(llvm::Value* lhs, llvm::Value* rhs);

    llvm::Value* generateEqual(llvm::Value* lhs, llvm::Value* rhs);

    llvm::Value* generateNotEqual(llvm::Value* lhs, llvm::Value* rhs);

    llvm::Value* generateLogicalAnd(llvm::Value* lhs, llvm::Value* rhs);

    llvm::Value* generateLogicalOr(llvm::Value* lhs, llvm::Value* rhs);

    llvm::Value* generateAssign(llvm::Value* lhs, llvm::Value* rhs);

    llvm::Value* generatePlusEqual(llvm::Value* lhs, llvm::Value* rhs);

    void generate(Visitor* v, const BinaryOperation* node);
}
