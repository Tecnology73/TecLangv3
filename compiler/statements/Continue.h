#pragma once

#include <llvm/IR/Value.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Verifier.h>
#include "../../ast/Statements.h"
#include "../Compiler.h"
#include "../context/ForLoopContext.h"

llvm::Value *generateContinue(Visitor *v, Continue *node) {
    auto context = Compiler::getScopeManager().findContext<ForLoopContext>();
    if (!context) {
        // v->PrintError("Continue statement must be inside a for loop");
        return nullptr;
    }

    context->handleContinue(node);
    return nullptr;
}