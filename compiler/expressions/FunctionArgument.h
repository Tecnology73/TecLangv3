#pragma once

#include <llvm/IR/Value.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Verifier.h>
#include "../../ast/Expressions.h"

llvm::Value *generateFunctionParameter(Visitor *v, FunctionParameter *param) {
    /*param->alloc = Compiler::getBuilder().CreateLoad(
        param->type->compiledType->finalType,
        param->statement->llvmFunction->getArg(param->index),
        param->name
    );*/
    param->alloc = param->function->llvmFunction->getArg(param->index);

    Compiler::getScopeManager().add(param);

    return param->alloc;
}