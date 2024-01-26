#pragma once

#include "../Compiler.h"
#include "../TypeCoercion.h"
#include "../../context/compiler/ReturnCompilerContext.h"

void generateReturn(Visitor* v, Return* ret) {
    auto returnType = Compiler::getScopeManager().getContext()->getReturnType();
    if (!returnType) {
        v->ReportError(ErrorCode::SYNTAX_ERROR, {"return"}, ret);
        return;
    }

    if (!ret->expression) {
        Compiler::getScopeManager().getContext()->handleReturn(ret);
        v->AddSuccess();
        return;
    }

    Compiler::getScopeManager().pushContext(new ReturnCompilerContext(v, ret));
    ret->expression->Accept(v);
    Compiler::getScopeManager().popContext();

    VisitorResult result;
    if (!v->TryGetResult(result)) return;

    // This happens with `return when` statements because
    // they create their own return instructions when inside a ReturnContext.
    if (!result.value) return;

    auto coercedValue = TypeCoercion::coerce(result.value, returnType->ResolveType()->llvmType);
    if (!coercedValue) {
        // FIXME: This should probably be caught during semantic analysis.
        v->ReportError(
            ErrorCode::TYPE_COERCION_IMPLICIT_FAILED,
            {
                result.type->name,
                returnType->name
            },
            ret->expression
        );
        return;
    }

    if (coercedValue->getType()->isPointerTy())
        coercedValue = Compiler::getBuilder().CreateLoad(returnType->ResolveType()->llvmType, coercedValue);

    Compiler::getScopeManager().getContext()->handleReturn(ret, coercedValue);
    v->AddSuccess();
}
