#include "Function.h"
#include <llvm/IR/Value.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Verifier.h>
#include "../Compiler.h"
#include "../topLevel/TypeBase.h"
#include "../../context/compiler/FunctionCompilerContext.h"

void generateFunction(Visitor* v, Function* func) {
    if (func->llvmFunction) {
        v->AddSuccess(func->llvmFunction);
        return;
    }

    auto lastBlock = Compiler::getBuilder().GetInsertBlock();

    // Generate parameters
    std::vector<llvm::Type *> parameters;
    for (auto& paramName: func->parameterOrder) {
        auto paramType = func->parameters[paramName]->type->ResolveType()->getLlvmType();
        // if (paramType)
        // paramType = paramType->getPointerTo();

        parameters.push_back(paramType);
    }

    // Define the function.
    auto returnType = TypeCompiler::compile(func->returnType);
    if (!func->returnType->ResolveType()->isValueType && !returnType->isPointerTy())
        returnType = returnType->getPointerTo();

    llvm::FunctionType* funcType = llvm::FunctionType::get(
        returnType,
        parameters,
        func->isExternal
    );

    // Create the function.
    auto linkage = llvm::Function::PrivateLinkage;
    if (func->isExternal || func->name == "main")
        linkage = llvm::Function::ExternalLinkage;

    auto funcName = std::string(func->name);
    if (func->ownerType)
        funcName = func->ownerType->name + "_" + funcName;

    llvm::Function* function = func->llvmFunction = llvm::Function::Create(
                                   funcType,
                                   linkage,
                                   funcName.data(),
                                   Compiler::getModule()
                               );

    if (!func->isExternal) {
        // Create a new basic block to start insertion into.
        auto basicBlock = llvm::BasicBlock::Create(
            Compiler::getContext(),
            "entry",
            function
        );

        Compiler::getBuilder().SetInsertPoint(basicBlock);

        // Create a scope
        Compiler::getScopeManager().enter(func->name, new FunctionCompilerContext(v, func));

        // Generate the parameters.
        for (const auto& paramName: func->parameterOrder) {
            func->parameters[paramName]->Accept(v);

            if (VisitorResult result; !v->TryGetResult(result)) return;
        }

        // Generate the body.
        for (auto& statement: func->body) {
            statement->Accept(v);

            if (VisitorResult result; !v->TryGetResult(result)) return;
        }

        // Close the scope
        Compiler::getScopeManager().popContext();
        Compiler::getScopeManager().leave(std::string(func->name));
    }

    // Validate the generated code, checking for consistency.
    llvm::verifyFunction(*function);

    // Reset the builder.
    Compiler::getBuilder().SetInsertPoint(lastBlock);

    v->AddSuccess(function);
}
