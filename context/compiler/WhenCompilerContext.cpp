#include "WhenCompilerContext.h"
#include "VarDeclarationCompilerContext.h"
#include "ReturnCompilerContext.h"
#include "../../compiler/Compiler.h"
#include "../../compiler/TypeCoercion.h"

WhenCompilerContext::WhenCompilerContext(Visitor* visitor, When* when) : Context(visitor), when(when) {
}

TypeReference* WhenCompilerContext::getReturnType() {
    if (auto varContext = dynamic_cast<VarDeclarationCompilerContext *>(parent))
        return varContext->var->type;

    if (auto returnContext = dynamic_cast<ReturnCompilerContext *>(parent))
        return returnContext->getReturnType();

    return nullptr;
}

void WhenCompilerContext::handleReturn(Node* node, llvm::Value* value) {
    if (auto varContext = dynamic_cast<VarDeclarationCompilerContext *>(parent)) {
        auto coercedValue = TypeCoercion::coerce(value, varContext->var->type->ResolveType()->llvmType);
        if (!coercedValue) {
            // FIXME: This should probably be caught during semantic analysis.
            visitor->ReportError(
                ErrorCode::TYPE_COERCION_IMPLICIT_FAILED,
                {
                    // Compiler::getScopeManager().getType(value->getType())->name.c_str(),
                    "",
                    varContext->var->type->ResolveType()->name
                },
                node
            );
            return;
        }

        Compiler::getBuilder().CreateStore(coercedValue, varContext->var->alloc);
    } else if (auto returnContext = dynamic_cast<ReturnCompilerContext *>(parent)) {
        auto returnType = returnContext->getReturnType();
        auto coercedValue = TypeCoercion::coerce(value, returnType->ResolveType()->llvmType);
        if (!coercedValue) {
            // FIXME: This should probably be caught during semantic analysis.
            visitor->ReportError(
                ErrorCode::TYPE_COERCION_IMPLICIT_FAILED,
                {
                    // Compiler::getScopeManager().getType(value->getType())->name.c_str(),
                    "",
                    returnType->name
                },
                node
            );
            return;
        }

        returnContext->handleReturn(node, coercedValue);
        return; // Don't create a branch to the exit block as it'd be unreachable at this point anyway.
    }

    Compiler::getBuilder().CreateBr(exitBlock);
}

bool WhenCompilerContext::isParentReturn() const {
    return dynamic_cast<ReturnCompilerContext *>(parent) != nullptr;
}
