#pragma once

#include <llvm/IR/Value.h>
#include "../Compiler.h"
#include "../../context/compiler/VarDeclarationCompilerContext.h"

namespace {
    void genCstrCall(Visitor* v, VariableDeclaration* node) {
        node->alloc = Compiler::getBuilder().CreateCall(
            Compiler::getScopeManager().getFunction("malloc")->llvmFunction,
            Compiler::getBuilder().getInt32(
                Compiler::getModule().getDataLayout().getTypeAllocSize(node->type->ResolveType()->type->getLlvmType())
            )
        );

        // Call expression
        if (node->expression) {
            Compiler::getScopeManager().enter(std::string(node->name), new VarDeclarationCompilerContext(v, node));

            // Some expressions (like 'when') don't return an expression directly.
            node->expression->Accept(v);
            if (!v->GetResult().success) {
                v->AddFailure();
                return;
            }

            /*if (auto expr = node->expression->Accept(v))
                Compiler::getBuilder().CreateStore(
                    TypeCoercion::coerce(expr, node->type->llvmType),
                    node->alloc
                );*/

            // Cleanup
            Compiler::getScopeManager().popContext();
            Compiler::getScopeManager().leave(node->name);
        }

        Compiler::getScopeManager().Add(node);

        v->AddSuccess(node->alloc);
    }
}

void generateVariableDeclaration(Visitor* v, VariableDeclaration* node) {
    if (node->alloc) {
        v->AddSuccess(node->alloc);
        return;
    }

    // Ensure the type has been generated.
    auto nodeType = node->type->ResolveType()->type;
    generateTypeDefinition(v, nodeType);

    if (Compiler::getScopeManager().HasVar(node->name)) {
        v->ReportError(ErrorCode::VARIABLE_ALREADY_DECLARED, {node->name}, node);
        return;
    }

    // If the expression is a constant, we don't need to do any allocations.
    /*if (!node->type->isStruct && dynamic_cast<Literal *>(node->expression)) {
        node->alloc = node->expression->Accept(v);
        Compiler::getScopeManager().add(node);

        return node->alloc;
    }*/

    if (dynamic_cast<ConstructorCall *>(node->expression)) {
        genCstrCall(v, node);
        return;
    }

    // if (node->type->isStruct) {
    /*if (false) {
        if (node->expression) {
            node->expression->Accept(v);
            auto result = v->GetResult();
            if (!result.success) {
                v->AddFailure();
                return;
            }

            node->alloc = result.value;
        }
    } else {*/
    node->alloc = Compiler::getBuilder().CreateAlloca(
        nodeType->llvmType,
        nullptr,
        node->name
    );

    // Generate the expression.
    if (node->expression) {
        Compiler::getScopeManager().enter(node->name, new VarDeclarationCompilerContext(v, node));

        // Some expressions (like when) don't return an expression directly.
        node->expression->Accept(v);
        if (auto result = v->GetResult(); result.success)
            Compiler::getBuilder().CreateStore(
                TypeCoercion::coerce(result.value, nodeType->llvmType),
                node->alloc
            );

        // Cleanup
        Compiler::getScopeManager().popContext();
        Compiler::getScopeManager().leave(node->name);
    } else {
        // Assign the default expression for the type.
        Compiler::getBuilder().CreateStore(nodeType->getDefaultValue(), node->alloc);
    }
    // }

    Compiler::getScopeManager().Add(node);
    v->AddSuccess(node->alloc);
}
