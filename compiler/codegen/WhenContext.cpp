#include "WhenContext.h"
#include "VarDeclarationContext.h"
#include "../Compiler.h"
#include "../TypeCoercion.h"
#include "ReturnContext.h"

WhenContext::WhenContext(Visitor *visitor, When *when) : CodegenContext(visitor), when(when) {}

TypeDefinition *WhenContext::getReturnType() {
    if (auto varContext = dynamic_cast<VarDeclarationContext *>(parent))
        return dynamic_cast<TypeDefinition *>(varContext->var->type);
    else if (auto returnContext = dynamic_cast<ReturnContext *>(parent))
        return returnContext->getReturnType();

    return nullptr;
}

void WhenContext::handleReturn(const Node *node, llvm::Value *value) {
    if (auto varContext = dynamic_cast<VarDeclarationContext *>(parent)) {
        auto coercedValue = TypeCoercion::coerce(value, varContext->var->type->llvmType);
        if (!coercedValue) {
            visitor->ReportError(
                ErrorCode::TYPE_COERCION_IMPLICIT_FAILED,
                {
                    Compiler::getScopeManager().getType(value->getType())->name.c_str(),
                    varContext->var->type->name.c_str()
                },
                node
            );
            return;
        }

        Compiler::getBuilder().CreateStore(coercedValue, varContext->var->alloc);
    } else if (auto returnContext = dynamic_cast<ReturnContext *>(parent)) {
        auto returnType = returnContext->getReturnType();
        auto coercedValue = TypeCoercion::coerce(value, returnType->llvmType);
        if (!coercedValue) {
            visitor->ReportError(
                ErrorCode::TYPE_COERCION_IMPLICIT_FAILED,
                {
                    Compiler::getScopeManager().getType(value->getType())->name.c_str(),
                    returnType->name.c_str()
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

bool WhenContext::isParentReturn() const {
    return dynamic_cast<ReturnContext *>(parent) != nullptr;
}
