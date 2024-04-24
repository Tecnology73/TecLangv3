#include "FunctionCall.h"
#include "VariableReference.h"
#include "../TypeCoercion.h"
#include "../../context/compiler/FunctionCompilerContext.h"
#include "../../ast/literals/String.h"
#include "../../scope/Scope.h"

void generateTypeFunctionCall(
    Visitor* v,
    const TypeBase* parentType,
    const FunctionCall* var,
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
    std::vector<llvm::Value*> arguments{
        value // this
    };
    for (auto& argument: var->arguments) {
        argument->Accept(v);

        VisitorResult result;
        if (!v->TryGetResult(result)) return;

        // String literals are stored as a global.
        // Inside a function, we normally load the value if it's a pointer (which it is in the case of a string).
        // This is a bit of a hack to get around that.
        if (dynamic_cast<String*>(argument)) {
            auto tmp = Compiler::getBuilder().CreateAlloca(result.value->getType());
            Compiler::getBuilder().CreateStore(result.value, tmp);

            arguments.push_back(tmp);
            continue;
        }

        arguments.push_back(result.value);
    }

    // Call statement
    func->Accept(v);

    VisitorResult result;
    if (!v->TryGetResult(result)) return;

    value = Compiler::getBuilder().CreateCall(static_cast<llvm::Function*>(result.value), arguments, "");

    // Recurse
    if (var->next) {
        if (auto function = dynamic_cast<FunctionCall*>(var->next)) {
            generateTypeFunctionCall(v, func->returnType->ResolveType(), function, value);
            return;
        }

        getValueFromType(v, func->returnType->ResolveType(), var->next, var, value);
        return;
    }

    v->AddSuccess(value);
}

void tryGenerateWithThisPrefix(Visitor* v, const FunctionCall* node) {
    // This allows for accessing fields of the current type without prefixing it with 'this.'
    auto context = Scope::FindContext<FunctionCompilerContext>();
    if (!context) {
        v->ReportError(ErrorCode::FUNCTION_UNKNOWN, {node->name}, node);
        return;
    }

    auto currentFunction = context->function;
    if (!currentFunction->ownerType || currentFunction->ownerType->isValueType) {
        v->ReportError(ErrorCode::FUNCTION_UNKNOWN, {node->name}, node);
        return;
    }

    // Check if the "this" parameter exists.
    auto param = currentFunction->GetParameter("this");
    if (!param) {
        v->ReportError(ErrorCode::FUNCTION_UNKNOWN, {node->name}, node);
        return;
    }

    // Make sure the statement exists on the type.
    auto function = currentFunction->ownerType->GetFunction(node->name);
    if (!function) {
        v->ReportError(ErrorCode::FUNCTION_UNKNOWN, {node->name}, node);
        return;
    }

    generateTypeFunctionCall(v, currentFunction->ownerType, node, param->alloc);
}

void generateFunctionCall(Visitor* v, const FunctionCall* node) {
    // Generate parameters
    std::vector<llvm::Value*> arguments;
    for (auto& argument: node->arguments) {
        argument->Accept(v);

        VisitorResult result;
        if (!v->TryGetResult(result)) return;

        auto arg = node->function->GetParameter(arguments.size());
        if (result.value->getType()->isPointerTy() && dynamic_cast<VariableReference*>(argument)) {
            // VariableReference does not load the right-most node.
            result.value = Compiler::getBuilder().CreateLoad(result.type->ResolveType()->getLlvmType(), result.value);
        }

        auto value = TypeCoercion::coerce(
            result.value,
            arg->type->ResolveType()->getLlvmType()
        );
        arguments.push_back(value);
    }

    // Ensure the function has been compiled.
    // FIXME: This will break if the functions call each other...
    node->function->Accept(v);
    if (VisitorResult result; !v->TryGetResult(result)) return;

    auto value = Compiler::getBuilder().CreateCall(
        node->function->llvmFunction,
        arguments,
        // Without this being named, it breaks a certain case of having a break
        // just before a statement call inside a for loop... Fuck knows why. 😕
        node->function->llvmFunction->getReturnType()->isVoidTy() ? "" : node->name
    );

    if (auto func = dynamic_cast<FunctionCall*>(node->next)) {
        generateTypeFunctionCall(v, node->function->returnType->ResolveType(), func, value);
        return;
    }

    getValueFromType(v, node->function->returnType->ResolveType(), node->next, node, value);
}
