#include "VariableReference.h"
#include "../Compiler.h"
#include "FunctionCall.h"
#include "../topLevel/Enum.h"
#include "../codegen/FunctionContext.h"

llvm::Value *getValueFromType(
    Visitor *v,
    TypeDefinition *parentType,
    ChainableNode *var,
    llvm::Value *value
) {
    if (!var) return value;

    auto fieldType = parentType->getFieldTypeDef(var->name);
    auto actualType = fieldType->llvmType;
    if (fieldType->isStruct)
        actualType = actualType->getPointerTo();

    value = Compiler::getBuilder().CreateStructGEP(
        parentType->llvmType,
        value,
        parentType->getFieldIndex(var->name),
        var->name
    );

    if (var->next) {
        value = Compiler::getBuilder().CreateLoad(actualType, value, var->name);

        if (auto function = dynamic_cast<FunctionCall *>(var->next))
            return generateTypeFunctionCall(v, fieldType, function, value);

        return getValueFromType(v, fieldType, var->next, value);
    }

    return value;
}

llvm::Value *tryGenerateWithThisPrefix(Visitor *v, VariableReference *var) {
    // This allows for accessing fields of the current type without prefixing it with 'this.'
    auto context = Compiler::getScopeManager().findContext<FunctionContext>();
    if (!context) return nullptr;

    auto currentFunction = context->function;
    if (!currentFunction->ownerType || !currentFunction->ownerType->isStruct) return nullptr;

    // Check if the "this" parameter exists.
    auto param = currentFunction->getParameter("this");
    if (!param) return nullptr;

    // Make sure the field exists on the type.
    auto field = currentFunction->ownerType->getField(var->name);
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

    // If the variable is a constant, just return the value.
    if (!variable->type->isStruct) {
        if (value->getType()->isPointerTy())
            return Compiler::getBuilder().CreateLoad(variable->type->llvmType, value, var->name);

        return value;
    }

    if (auto function = dynamic_cast<FunctionCall *>(var->next))
        return generateTypeFunctionCall(v, dynamic_cast<TypeDefinition *>(variable->type), function, value);

    return getValueFromType(v, dynamic_cast<TypeDefinition *>(variable->type), var->next, value);
}