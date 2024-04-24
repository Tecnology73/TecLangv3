#include "VariableDeclaration.h"
#include "../Compiler.h"
#include "../TypeCoercion.h"
#include "../topLevel/TypeBase.h"
#include "../../ast/literals/String.h"
#include "../../ast/expressions/ConstructorCall.h"
#include "../../symbolTable/SymbolTable.h"
#include "../../context/compiler/VarDeclarationCompilerContext.h"
#include "../../scope/Scope.h"

void generateConstructorCall(Visitor* v, VariableDeclaration* node) {
    node->alloc = Compiler::getBuilder().CreateCall(
        SymbolTable::GetInstance()->LookupFunction("malloc", {})->llvmFunction,
        Compiler::getBuilder().getInt64(
            Compiler::getModule().getDataLayout().getTypeAllocSize(node->type->ResolveType()->getLlvmType())
        )
    );

    // Call expression
    if (node->expression) {
        auto [scope, context] = Scope::Enter<VarDeclarationCompilerContext>(v, node);

        // Some expressions (like 'when') don't return an expression directly.
        node->expression->Accept(v);
        if (VisitorResult result; !v->TryGetResult(result)) return;

        // Cleanup
        scope->Leave();
    }

    // Compiler::getScopeManager().Add(node);
    Scope::GetScope()->Add(node);

    v->AddSuccess(node->alloc);
}

void generateVariableDeclaration(Visitor* v, VariableDeclaration* node) {
    if (node->alloc) {
        v->AddSuccess(node->alloc);
        return;
    }

    // Ensure the type has been generated.
    auto nodeType = node->type->ResolveType();
    TypeCompiler::compile(node->type);

    // If the expression is a constant, we don't need to do any allocations.
    /*if (!node->type->isStruct && dynamic_cast<Literal *>(node->expression)) {
        node->alloc = node->expression->Accept(v);
        Scope::GetScope()->Add(node);

        return node->alloc;
    }*/

    if (dynamic_cast<ConstructorCall*>(node->expression)) {
        generateConstructorCall(v, node);
        return;
    }

    node->alloc = Compiler::getBuilder().CreateAlloca(
        nodeType->llvmType,
        nullptr,
        node->name
    );

    // Generate the expression.
    if (node->expression) {
        auto [scope, context] = Scope::Enter<VarDeclarationCompilerContext>(v, node);

        // Some expressions (like when) don't return an expression directly.
        node->expression->Accept(v);

        VisitorResult result;
        if (!v->TryGetResult(result)) return;

        if (nodeType->name == "string") {
            if (!dynamic_cast<ConstructorCall*>(node->expression))
                Compiler::getBuilder().CreateCall(
                    nodeType->GetFunction("construct")->llvmFunction,
                    {node->alloc}
                );

            if (dynamic_cast<String*>(node->expression)) {
                auto tmp = Compiler::getBuilder().CreateAlloca(result.value->getType());
                Compiler::getBuilder().CreateStore(result.value, tmp);

                Compiler::getBuilder().CreateCall(
                    nodeType->GetFunction("assign")->llvmFunction,
                    {node->alloc, tmp}
                );
            }
        } else if (result.value) {
            Compiler::getBuilder().CreateStore(
                TypeCoercion::coerce(result.value, nodeType->llvmType),
                node->alloc
            );
        }

        // Cleanup
        scope->Leave();
    } else {
        // Assign the default expression for the type.
        Compiler::getBuilder().CreateStore(TypeCompiler::getDefaultValue(node->type), node->alloc);
    }

    Scope::GetScope()->Add(node);
    v->AddSuccess(node->alloc);
}
