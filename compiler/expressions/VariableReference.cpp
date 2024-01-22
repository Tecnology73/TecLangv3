#include "VariableReference.h"
#include "../Compiler.h"
#include "FunctionCall.h"
#include "../topLevel/Enum.h"
#include "../../context/compiler/FunctionCompilerContext.h"

void getValueFromType(
    Visitor* v,
    const TypeBase* parentType,
    const ChainableNode* var,
    llvm::Value* value
) {
    if (!var) {
        v->AddSuccess(value);
        return;
    }

    auto fieldType = parentType->GetField(var->name)->type;
    auto actualType = fieldType->ResolveType()->type->llvmType;
    if (!fieldType->ResolveType()->type->isValueType)
        actualType = actualType->getPointerTo();

    value = Compiler::getBuilder().CreateStructGEP(
        parentType->llvmType,
        value,
        parentType->GetFieldIndex(var->name),
        var->name
    );

    if (var->next) {
        value = Compiler::getBuilder().CreateLoad(actualType, value, var->name);

        if (auto function = dynamic_cast<FunctionCall *>(var->next)) {
            generateTypeFunctionCall(v, fieldType->ResolveType()->type, function, value);
            return;
        }

        getValueFromType(v, fieldType->ResolveType()->type, var->next, value);
        return;
    }

    v->AddSuccess(value, fieldType->ResolveType());
}

void tryGenerateWithThisPrefix(Visitor* v, const VariableReference* var) {
    // This allows for accessing fields of the current type without prefixing it with 'this.'
    auto context = Compiler::getScopeManager().findContext<FunctionCompilerContext>();
    if (!context) {
        v->AddFailure();
        return;
    }

    auto currentFunction = context->function;
    if (!currentFunction->ownerType || currentFunction->ownerType->isValueType) {
        v->AddFailure();
        return;
    }

    // Check if the "this" parameter exists.
    auto param = currentFunction->GetParameter("this");
    if (!param) {
        v->AddFailure();
        return;
    }

    // Make sure the field exists on the type.
    auto field = currentFunction->ownerType->GetField(var->name);
    if (!field) {
        v->AddFailure();
        return;
    }

    getValueFromType(v, currentFunction->ownerType, var, param->alloc);
}

void generateVariableReference(Visitor* v, VariableReference* var) {
    auto symbol = Compiler::getScopeManager().GetVar(var->name);
    if (!symbol) {
        tryGenerateWithThisPrefix(v, var);
        return;
    }

    auto variable = dynamic_cast<const VariableDeclaration *>(symbol->node);
    if (!variable) {
        v->AddFailure();
        return;
    }

    auto value = variable->alloc;
    if (!value) {
        v->AddFailure();
        return;
    }

    // If the variable is a constant, just return the expression.
    if (variable->type->ResolveType()->type->isValueType) {
        v->AddSuccess(value);
        return;
    }

    if (auto function = dynamic_cast<FunctionCall *>(var->next)) {
        generateTypeFunctionCall(v, variable->type->ResolveType()->type, function, value);
        return;
    }

    getValueFromType(v, variable->type->ResolveType()->type, var->next, value);
}
