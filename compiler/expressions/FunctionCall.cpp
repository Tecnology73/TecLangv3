#include "FunctionCall.h"
#include "../Compiler.h"
#include "VariableReference.h"
#include "../context/FunctionContext.h"

llvm::Value *generateTypeFunctionCall(
    Visitor *v,
    TypeBase *parentType,
    FunctionCall *var,
    llvm::Value *value
) {
    if (!var) return value;

    auto func = parentType->GetFunction(var->name);
    if (!func) {
        v->ReportError(ErrorCode::TYPE_UNKNOWN_FUNCTION, {var->name, parentType->name}, var);
        return nullptr;
    }

    // Generate parameters
    std::vector<llvm::Value *> arguments{
        value // this
    };
    for (auto &argument: var->arguments)
        arguments.push_back(argument->Accept(v));

    // Call statement
    value = Compiler::getBuilder().CreateCall(
        (llvm::Function *) func->Accept(v),
        arguments,
        ""
    );

    // Recurse
    if (var->next) {
        if (auto function = dynamic_cast<FunctionCall *>(var->next))
            // return expression;
            return generateTypeFunctionCall(v, func->returnType, function, value);

        return getValueFromType(v, func->returnType, var->next, value);
    }

    return value;
}

llvm::Value *tryGenerateWithThisPrefix(Visitor *v, FunctionCall *node) {
    // This allows for accessing fields of the current type without prefixing it with 'this.'
    auto context = Compiler::getScopeManager().findContext<FunctionContext>();
    if (!context) return nullptr;

    auto currentFunction = context->function;
    if (!currentFunction->ownerType || currentFunction->ownerType->isValueType) return nullptr;

    // Check if the "this" parameter exists.
    auto param = currentFunction->GetParameter("this");
    if (!param) return nullptr;

    // Make sure the statement exists on the type.
    auto function = currentFunction->ownerType->GetFunction(node->name);
    if (!function) return nullptr;

    return generateTypeFunctionCall(v, currentFunction->ownerType, node, param->alloc);
}

llvm::Value *generateFunctionCall(Visitor *v, FunctionCall *node) {
    auto function = Compiler::getScopeManager().getFunction(node->name);
    if (!function) {
        // See if the variable exists if we prefix it with 'this.'
        auto thisValue = tryGenerateWithThisPrefix(v, node);
        if (!thisValue) {
            v->ReportError(ErrorCode::FUNCTION_UNKNOWN, {node->name}, node);
            return nullptr;
        }

        return thisValue;
    }

    // Generate parameters
    std::vector<llvm::Value *> arguments;
    for (auto &argument: node->arguments) {
        auto value = argument->Accept(v);
        if (value->getType()->isPointerTy() && dynamic_cast<VariableReference *>(argument)) {
            // VariableReference does not load the right-most node.
            value = Compiler::getBuilder().CreateLoad(value->getType(), value);
        }

        arguments.push_back(value);
    }

    auto value = Compiler::getBuilder().CreateCall(
        function->llvmFunction,
        arguments,
        // Without this being named, it breaks a certain case of having a break
        // just before a statement call inside a for loop... Fuck knows why. 😕
        function->name
    );

    if (auto func = dynamic_cast<FunctionCall *>(node->next))
        return generateTypeFunctionCall(v, function->returnType, func, value);

    return getValueFromType(v, function->returnType, node->next, value);
}
