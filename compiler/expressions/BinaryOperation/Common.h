#pragma once

#include <llvm/IR/Value.h>
#include "../../../ast/Expressions.h"

namespace BinaryOperation_Common {
    llvm::Value* generateValue(Visitor* v, const BinaryOperation* node, Node* valueNode, bool ensureLoaded = false);
}
