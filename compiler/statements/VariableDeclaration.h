#pragma once

#include <llvm/IR/Value.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Verifier.h>
#include "../../ast/Statements.h"
#include "../Compiler.h"
#include "TypeDefinition.h"
#include "../context/VarDeclarationContext.h"

llvm::Value *generateVariableDeclaration(Visitor *v, VariableDeclaration *node) {
    if (node->alloc)
        return node->alloc;

    // Ensure the type has been inferred and generated.
    node->GetCompiledType(v);
    if (!node->type) {
        v->ReportError(ErrorCode::VARIABLE_INFER_FAILED, {node->name}, node);
        return nullptr;
    }

    if (Compiler::getScopeManager().hasVar(node->name)) {
        v->ReportError(ErrorCode::VARIABLE_ALREADY_DECLARED, {node->name}, node);
        return nullptr;
    }

    // If the expression is a constant, we don't need to do any allocations.
    /*if (!node->type->isStruct && dynamic_cast<Literal *>(node->expression)) {
        node->alloc = node->expression->Accept(v);
        Compiler::getScopeManager().add(node);

        return node->alloc;
    }*/

    // if (node->type->isStruct) {
    if (false) {
        if (node->expression)
            node->alloc = node->expression->Accept(v);
    } else {
        node->alloc = Compiler::getBuilder().CreateAlloca(
            node->type->llvmType,
            nullptr,
            node->name
        );

        // Assign the default value for the type.
        Compiler::getBuilder().CreateStore(node->type->getDefaultValue(), node->alloc);

        // Generate the expression.
        if (node->expression) {
            Compiler::getScopeManager().enter(node->name, new VarDeclarationContext(v, node));

            auto expr = node->expression->Accept(v);
            // Some expressions (like when) don't return a value directly.
            if (expr)
                Compiler::getBuilder().CreateStore(
                    TypeCoercion::coerce(expr, node->type->llvmType),
                    node->alloc
                );

            // Cleanup
            Compiler::getScopeManager().popContext();
            Compiler::getScopeManager().leave(node->name);
        }
    }

    Compiler::getScopeManager().add(node);
    return node->alloc;
}