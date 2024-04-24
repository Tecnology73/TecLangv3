#pragma once

#include <llvm/IR/Value.h>
#include "../../ast/topLevel/Function.h"
#include "../../scope/Scope.h"

void generateFunctionParameter(Visitor* v, FunctionParameter* param) {
    param->alloc = param->function->llvmFunction->getArg(param->index);

    Scope::GetScope()->Add(param);

    v->AddSuccess(param->alloc);
}
