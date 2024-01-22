#pragma once

#include <llvm/IR/Verifier.h>
#include "../../ast/Expressions.h"

void generateTypeFunctionCall(
    Visitor* v,
    const TypeBase* parentType,
    FunctionCall* var,
    llvm::Value* value
);

void generateFunctionCall(Visitor* v, FunctionCall* node);
