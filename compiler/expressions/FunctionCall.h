#pragma once

#include <llvm/IR/Value.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Verifier.h>
#include "../../ast/Expressions.h"

llvm::Value *generateTypeFunctionCall(
    Visitor *v,
    TypeDefinition *parentType,
    FunctionCall *var,
    llvm::Value *value
);

llvm::Value *generateFunctionCall(Visitor *v, FunctionCall *node);