#pragma once

#include <llvm/IR/Value.h>
#include "../../ast/Expressions.h"

void getValueFromType(
    Visitor* v,
    TypeBase* parentType,
    const ChainableNode* var,
    const ChainableNode* prevVar,
    llvm::Value* value
);

void generateVariableReference(Visitor* v, const VariableReference* var);
