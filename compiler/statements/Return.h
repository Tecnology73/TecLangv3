#pragma once

#include "../../ast/Statements.h"
#include "../Compiler.h"
#include "../TypeCoercion.h"
#include "../codegen/ReturnContext.h"

llvm::ReturnInst *generateReturn(Visitor *v, Return *ret) {
    auto returnType = Compiler::getScopeManager().getContext()->getReturnType();
    if (!returnType) {
        v->ReportError(ErrorCode::SYNTAX_ERROR, {"return"}, ret);
        return nullptr;
    }

    Compiler::getScopeManager().pushContext(new ReturnContext(v, ret));
    auto value = ret->expression->Accept(v);
    Compiler::getScopeManager().popContext();
    if (!value)
        return nullptr;

    auto coercedValue = TypeCoercion::coerce(value, returnType->llvmType);
    if (!coercedValue) {
        v->ReportError(
            ErrorCode::TYPE_COERCION_IMPLICIT_FAILED,
            {
                Compiler::getScopeManager().getType(value->getType())->name.c_str(),
                returnType->name.c_str()
            },
            ret->expression
        );
        return nullptr;
    }

    if (coercedValue->getType()->isPointerTy())
        coercedValue = Compiler::getBuilder().CreateLoad(returnType->llvmType, coercedValue);

    Compiler::getScopeManager().getContext()->handleReturn(ret, coercedValue);
}