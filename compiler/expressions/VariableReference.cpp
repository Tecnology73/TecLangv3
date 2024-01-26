#include "VariableReference.h"
#include "../Compiler.h"
#include "FunctionCall.h"
#include "../topLevel/Enum.h"
#include "../../ast/literals/String.h"
#include "../../context/compiler/FunctionCompilerContext.h"

void getValueFromType(
    Visitor* v,
    const TypeBase* parentType,
    const ChainableNode* var,
    const ChainableNode* prevVar,
    llvm::Value* value
) {
    if (!var) {
        if (prevVar && prevVar->loadInternalData) {
            value = Compiler::getBuilder().CreateStructGEP(
                parentType->llvmType,
                value,
                parentType->GetFieldIndex("data"),
                "data"
            );
        }

        v->AddSuccess(value, parentType->CreateReference());
        return;
    }

    auto fieldType = parentType->GetField(var->name)->type;
    auto actualType = fieldType->ResolveType()->llvmType;
    if (!fieldType->ResolveType()->isValueType)
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
            generateTypeFunctionCall(v, fieldType->ResolveType(), function, value);
            return;
        }

        getValueFromType(v, fieldType->ResolveType(), var->next, var, value);
        return;
    }

    v->AddSuccess(value, fieldType);
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

    getValueFromType(v, currentFunction->ownerType, var, nullptr, param->alloc);
}

void generateVariableReference(Visitor* v, const VariableReference* var) {
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
    auto varType = variable->type;
    if (varType->ResolveType()->isValueType) {
        v->AddSuccess(value, varType);
        return;
    }

    if (auto function = dynamic_cast<FunctionCall *>(var->next)) {
        generateTypeFunctionCall(v, varType->ResolveType(), function, value);
        return;
    }

    getValueFromType(v, varType->ResolveType(), var->next, var, value);
}
