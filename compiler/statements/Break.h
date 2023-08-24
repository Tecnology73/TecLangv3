#pragma once

#include <llvm/IR/Value.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Verifier.h>
#include "../../ast/Statements.h"
#include "../Compiler.h"
#include "../context/ForLoopContext.h"

llvm::Value *generateBreak(Visitor *v, Break *node) {
    auto context = Compiler::getScopeManager().findContext<ForLoopContext>();
    if (!context) {
        // v->PrintError("Continue statement must be inside a for loop");
        return nullptr;
    }

    context->handleBreak(node);
    return nullptr;
}