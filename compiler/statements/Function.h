#pragma once

#include <llvm/IR/Value.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Verifier.h>
#include "../Compiler.h"
#include "../../context/compiler/FunctionCompilerContext.h"

void generateFunction(Visitor* v, Function* func) {
    if (func->llvmFunction) {
        v->AddSuccess(func->llvmFunction);
        return;
    }

    auto lastBlock = Compiler::getBuilder().GetInsertBlock();

    // Generate parameters
    std::vector<llvm::Type *> parameters;
    for (auto& paramName: func->parameterOrder)
        parameters.push_back(func->parameters[paramName]->type->ResolveType()->type->getLlvmType()->getPointerTo());

    // Define the function.
    auto returnType = generateTypeDefinition(v, func->returnType->ResolveType()->type);
    if (!func->returnType->ResolveType()->type->isValueType)
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
        llvm::BasicBlock* basicBlock = llvm::BasicBlock::Create(
            Compiler::getContext(),
            "entry",
            function
        );

        // Create a new builder for the basic block.
        Compiler::getBuilder().SetInsertPoint(basicBlock);

        // Create a scope
        Compiler::getScopeManager().enter(std::string(func->name), new FunctionCompilerContext(v, func));

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

        // Ensure `void` functions return.
        if (function->getReturnType()->isVoidTy() && !function->end()->getTerminator())
            Compiler::getBuilder().CreateRetVoid();

        // Close the scope
        Compiler::getScopeManager().popContext();
        Compiler::getScopeManager().leave(std::string(func->name));
    }

    // Validate the generated code, checking for consistency.
    llvm::verifyFunction(*function);

    // Add the statement to the symbol table.
    if (!func->ownerType)
        Compiler::getScopeManager().add(func);

    // Reset the builder.
    Compiler::getBuilder().SetInsertPoint(lastBlock);

    v->AddSuccess(function);
}
