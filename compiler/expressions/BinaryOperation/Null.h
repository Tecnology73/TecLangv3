#pragma once

#include <llvm/IR/Value.h>
#include "../../../ast/Expressions.h"

namespace BinaryOperation_Null {
    llvm::Value* generateEqual(Visitor* v, const BinaryOperation* node);

    llvm::Value* generateNotEqual(Visitor* v, const BinaryOperation* node);

    llvm::Value* generateAssign(Visitor* v, const BinaryOperation* node);

    llvm::Value* generateIs(Visitor* v, const BinaryOperation* node);
}
