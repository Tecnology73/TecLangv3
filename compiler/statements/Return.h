#pragma once

#include "../../ast/Statements.h"
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

    auto coercedValue = TypeCoercion::coerce(result.value, returnType->type->llvmType);
    if (!coercedValue) {
        v->ReportError(
            ErrorCode::TYPE_COERCION_IMPLICIT_FAILED,
            {
                Compiler::getScopeManager().getType(result.value->getType())->name.c_str(),
                returnType->type->name.c_str()
            },
            ret->expression
        );
        return;
    }

    if (coercedValue->getType()->isPointerTy())
        coercedValue = Compiler::getBuilder().CreateLoad(returnType->type->llvmType, coercedValue);

    Compiler::getScopeManager().getContext()->handleReturn(ret, coercedValue);
    v->AddSuccess();
}
