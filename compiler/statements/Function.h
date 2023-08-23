#pragma once

#include <llvm/IR/Value.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Verifier.h>
#include "../../ast/Expressions.h"
#include "../Compiler.h"
#include "../codegen/FunctionContext.h"

llvm::Function *generateFunction(Visitor *v, Function *func) {
    if (func->llvmFunction)
        return func->llvmFunction;

    auto lastBlock = Compiler::getBuilder().GetInsertBlock();

    // Generate parameters
    std::vector<llvm::Type *> parameters;
    for (auto &param: func->parameters)
        parameters.push_back(generateTypeDefinition(v, param->type)->getPointerTo());

    // Define the statement.
    auto returnType = generateTypeDefinition(v, func->returnType);
    if (func->returnType->isStruct)
        returnType = returnType->getPointerTo();

    llvm::FunctionType *funcType = llvm::FunctionType::get(
        returnType,
        parameters,
        func->isExternal
    );

    // Create the statement.
    auto linkage = llvm::Function::PrivateLinkage;
    if (func->isExternal || func->name == "main")
        linkage = llvm::Function::ExternalLinkage;

    auto funcName = func->name;
    if (func->ownerType)
        funcName = func->ownerType->name + "_" + funcName;

    llvm::Function *function = func->llvmFunction = llvm::Function::Create(
        funcType,
        linkage,
        funcName,
        Compiler::getModule()
    );

    if (!func->isExternal) {
        // Create a new basic block to start insertion into.
        llvm::BasicBlock *basicBlock = llvm::BasicBlock::Create(
            Compiler::getContext(),
            "entry",
            function
        );

        // Create a new builder for the basic block.
        Compiler::getBuilder().SetInsertPoint(basicBlock);

        // Create a scope
        auto context = Compiler::getScopeManager().enter(func->name, new FunctionContext(v, func));

        // Generate the parameters.
        for (const auto &item: func->parameters)
            item->Accept(v);

        // Generate the body.
        for (auto &statement: func->body)
            statement->Accept(v);

        // Infer return type.
        // TODO: We need to somehow infer the return type before anything is generated
        //   because we need to make a `llvm::FunctionType` first.
        /*std::set<TypeDefinition *> returnTypes;
        for (const auto &item: context->returnStatements) {
            auto type = Compiler::getScopeManager().getType(item.first);
            if (!type) {
                v->ReportError(ErrorCode::UNKNOWN_ERROR, {}, item.second[0]);
                continue;
            }

            returnTypes.emplace(type);
        }

        if (returnTypes.size() != 1) {
            v->ReportError(ErrorCode::UNKNOWN_ERROR, {}, func);
            return nullptr;
        }*/

        // Close the scope
        Compiler::getScopeManager().popContext();
        Compiler::getScopeManager().leave(func->name);
    }

    // Validate the generated code, checking for consistency.
    llvm::verifyFunction(*function);

    // Add the statement to the symbol table.
    if (!func->ownerType)
        Compiler::getScopeManager().add(func);

    // Reset the builder.
    Compiler::getBuilder().SetInsertPoint(lastBlock);

    return function;
}