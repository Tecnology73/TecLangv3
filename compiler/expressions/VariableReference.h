#pragma once

#include <llvm/IR/Value.h>
#include "../../ast/Expressions.h"
#include "../../ast/Statements.h"

llvm::Value *getValueFromType(
    Visitor *v,
    TypeBase *parentType,
    ChainableNode *var,
    llvm::Value *value
);

llvm::Value *generateVariableReference(Visitor *v, VariableReference *var);