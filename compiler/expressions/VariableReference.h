#pragma once

#include <llvm/IR/Value.h>
#include "../../ast/Expressions.h"

void getValueFromType(
    Visitor* v,
    const TypeBase* parentType,
    const ChainableNode* var,
    llvm::Value* value
);

void generateVariableReference(Visitor* v, VariableReference* var);
