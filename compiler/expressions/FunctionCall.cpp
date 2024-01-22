#include "FunctionCall.h"
#include "../Compiler.h"
#include "VariableReference.h"
#include "../../context/compiler/FunctionCompilerContext.h"

void generateTypeFunctionCall(
    Visitor* v,
    const TypeBase* parentType,
    FunctionCall* var,
    llvm::Value* value
) {
    if (!var) {
        v->AddSuccess(value);
        return;
    }

    auto func = parentType->GetFunction(var->name);
    if (!func) {
        v->ReportError(ErrorCode::TYPE_UNKNOWN_FUNCTION, {var->name, parentType->name}, var);
        return;
    }

    // Generate parameters
    std::vector<llvm::Value *> arguments{
        value // this
    };
    for (auto& argument: var->arguments) {
        argument->Accept(v);

        VisitorResult result;
        if (!v->TryGetResult(result)) return;

        arguments.push_back(result.value);
    }

    // Call statement
    func->Accept(v);

    VisitorResult result;
    if (!v->TryGetResult(result)) return;

    value = Compiler::getBuilder().CreateCall(static_cast<llvm::Function *>(result.value), arguments, "");

    // Recurse
    if (var->next) {
        if (auto function = dynamic_cast<FunctionCall *>(var->next)) {
            generateTypeFunctionCall(v, func->returnType->ResolveType()->type, function, value);
            return;
        }

        getValueFromType(v, func->returnType->ResolveType()->type, var->next, value);
        return;
    }

    v->AddSuccess(value);
}

void tryGenerateWithThisPrefix(Visitor* v, FunctionCall* node) {
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

    // Make sure the statement exists on the type.
    auto function = currentFunction->ownerType->GetFunction(node->name);
    if (!function) {
        v->AddFailure();
        return;
    }

    generateTypeFunctionCall(v, currentFunction->ownerType, node, param->alloc);
}

void generateFunctionCall(Visitor* v, FunctionCall* node) {
    auto function = Compiler::getScopeManager().getFunction(node->name);
    if (!function) {
        tryGenerateWithThisPrefix(v, node);
        return;

        // See if the variable exists if we prefix it with 'this.'
        /*auto thisValue = tryGenerateWithThisPrefix(v, node);
        if (!thisValue) {
            v->ReportError(ErrorCode::FUNCTION_UNKNOWN, {node->name}, node);
            v->AddFailure();
            return;
        }

        v->AddSuccess(thisValue);
        return;*/
    }

    // Generate parameters
    std::vector<llvm::Value *> arguments;
    for (auto& argument: node->arguments) {
        argument->Accept(v);

        VisitorResult result;
        if (!v->TryGetResult(result)) return;

        if (result.value->getType()->isPointerTy() && dynamic_cast<VariableReference *>(argument)) {
            // VariableReference does not load the right-most node.
            result.value = Compiler::getBuilder().CreateLoad(result.value->getType(), result.value);
        }

        arguments.push_back(result.value);
    }

    auto value = Compiler::getBuilder().CreateCall(
        function->llvmFunction,
        arguments,
        // Without this being named, it breaks a certain case of having a break
        // just before a statement call inside a for loop... Fuck knows why. 😕
        function->llvmFunction->getReturnType()->isVoidTy() ? "" : function->name
    );

    if (auto func = dynamic_cast<FunctionCall *>(node->next)) {
        generateTypeFunctionCall(v, function->returnType->ResolveType()->type, func, value);
        return;
    }

    getValueFromType(v, function->returnType->ResolveType()->type, node->next, value);
}
