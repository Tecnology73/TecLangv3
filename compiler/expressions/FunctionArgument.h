#pragma once

#include <llvm/IR/Value.h>
#include "../../ast/topLevel/Function.h"

void generateFunctionParameter(Visitor *v, FunctionParameter *param) {
    param->alloc = param->function->llvmFunction->getArg(param->index);

    Compiler::getScopeManager().Add(param);

    v->AddSuccess(param->alloc);
}
