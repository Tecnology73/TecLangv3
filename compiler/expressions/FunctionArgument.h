#pragma once

#include <llvm/IR/Value.h>
#include "../../ast/topLevel/Function.h"

llvm::Value *generateFunctionParameter(Visitor *v, FunctionParameter *param) {
    /*param->alloc = param->function->llvmFunction->getArg(param->index);

    Compiler::getScopeManager().add(param);

    return param->alloc;*/
    return nullptr;
}