#pragma once

#include "../Compiler.h"
#include "../../context/compiler/ForLoopCompilerContext.h"

void generateContinue(Visitor* v, Continue* node) {
    auto context = Compiler::getScopeManager().findContext<ForLoopCompilerContext>();
    if (!context) {
        // v->PrintError("Continue statement must be inside a for loop");
        // v->AddFailure();
        return;
    }

    context->handleContinue(node);
    v->AddSuccess();
}
