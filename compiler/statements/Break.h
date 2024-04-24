#pragma once

#include "../Compiler.h"
#include "../../context/compiler/ForLoopCompilerContext.h"

void generateBreak(Visitor* v, Break* node) {
    auto context = Scope::FindContext<ForLoopCompilerContext>();
    if (!context) {
        // v->PrintError("Continue statement must be inside a for loop");
        // v->AddFailure();
        return;
    }

    context->handleBreak(node);
    v->AddSuccess();
}
