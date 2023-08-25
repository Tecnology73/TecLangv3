#include "VariableReference.h"
#include "../Compiler.h"
#include "FunctionCall.h"
#include "../topLevel/Enum.h"
#include "../context/FunctionContext.h"

llvm::Value *getValueFromType(
    Visitor *v,
    TypeBase *parentType,
    ChainableNode *var,
    llvm::Value *value
) {
    if (!var) return value;

    /*auto fieldType = parentType->getFieldType(var->name);
    auto actualType = fieldType->llvmType;
    if (fieldType->isStruct)
        actualType = actualType->getPointerTo();

    expression = Compiler::getBuilder().CreateStructGEP(
        parentType->llvmType,
        expression,
        parentType->GetFieldIndex(var->name),
        var->name
    );

    if (var->next) {
        expression = Compiler::getBuilder().CreateLoad(actualType, expression, var->name);

        if (auto function = dynamic_cast<FunctionCall *>(var->next))
            return generateTypeFunctionCall(v, fieldType, function, expression);

        return getValueFromType(v, fieldType, var->next, expression);
    }

    return expression;*/
    return nullptr;
}

llvm::Value *tryGenerateWithThisPrefix(Visitor *v, VariableReference *var) {
    // This allows for accessing fields of the current type without prefixing it with 'this.'
    auto context = Compiler::getScopeManager().findContext<FunctionContext>();
    if (!context) return nullptr;

    auto currentFunction = context->function;
    if (!currentFunction->ownerType || currentFunction->ownerType->isValueType) return nullptr;

    // Check if the "this" parameter exists.
    auto param = currentFunction->GetParameter("this");
    if (!param) return nullptr;

    // Make sure the field exists on the type.
    auto field = ((TypeDefinition *) currentFunction->ownerType)->GetField(var->name);
    if (!field) return nullptr;

    return getValueFromType(v, currentFunction->ownerType, var, param->alloc);
}

llvm::Value *generateVariableReference(Visitor *v, VariableReference *var) {
    auto variable = Compiler::getScopeManager().getVar(var->name);
    if (!variable) {
        // See if the variable exists if we prefix it with 'this.'
        auto thisValue = tryGenerateWithThisPrefix(v, var);
        if (!thisValue) {
            v->ReportError(ErrorCode::VARIABLE_UNDECLARED, {var->name}, var);
            return nullptr;
        }

        return thisValue;
    }

    auto value = variable->alloc;
    if (!value) return nullptr;

    // If the variable is a constant, just return the expression.
    if (variable->type->isValueType) {
        if (value->getType()->isPointerTy())
            return Compiler::getBuilder().CreateLoad(variable->type->llvmType, value, var->name);

        return value;
    }

    if (auto function = dynamic_cast<FunctionCall *>(var->next))
        return generateTypeFunctionCall(v, variable->type, function, value);

    return getValueFromType(v, variable->type, var->next, value);
}
