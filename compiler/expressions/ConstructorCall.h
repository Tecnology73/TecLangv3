#pragma once

#include <llvm/IR/Value.h>
#include <string>
#include "../Compiler.h"
#include "../statements/TypeDefinition.h"
#include "../ErrorManager.h"
#include "../TypeCoercion.h"
#include "../../context/compiler/VarDeclarationCompilerContext.h"

void generateConstructorCall(Visitor* v, const ConstructorCall* node) {
    auto nodeType = node->type->ResolveType();
    generateTypeDefinition(v, nodeType);

    // FIXME: This could probably be a little cleaner/efficient if we just pass in `node->parameters`
    //  to the statement lookup and (somehow) get the TypeDefinition from the node.
    // This would allow us to do any type coercion (or just override the bits for an integer?).
    // Perhaps we could just have a `TypeDefinition::callFunction` that does all the work for us?
    auto context = dynamic_cast<VarDeclarationCompilerContext *>(Compiler::getScopeManager().getContext());
    std::vector<llvm::Value *> args = {
        context->var->alloc
    };
    std::vector<TypeReference *> argTypes{
        context->var->type
    };
    for (const auto& item: node->arguments) {
        item->Accept(v);

        VisitorResult result;
        if (!v->TryGetResult(result)) return;

        args.push_back(result.value);
        argTypes.push_back(result.type);
    }

    // Find the constructor that matches the argTypes.
    auto constructFunction = nodeType->FindFunction("construct", args, argTypes);
    if (!constructFunction) {
        std::string argTypesStr;
        for (const auto& type: argTypes) {
            argTypesStr += type->name;
            if (&type != &argTypes.back()) argTypesStr += ", ";
        }

        v->ReportError(ErrorCode::TYPE_UNKNOWN_CONSTRUCTOR, {nodeType->name, argTypesStr}, node);
        return;
    }

    // Make sure that the parameters are all the same type as the statement parameters so LLVM doesn't complain.
    for (int i = 0; i < args.size(); i++)
        args[i] = TypeCoercion::coerce(
            args[i],
            constructFunction->GetParameter(i)->type->ResolveType()->llvmType
        );

    // Call the constructor
    Compiler::getBuilder().CreateCall(constructFunction->llvmFunction, args);

    // If this is actually "assignment constructor", do all the assignments.
    // a = new T { x = 1, y = 2 }
    for (const auto& item: node->fields) {
        // Make sure the field exists on the type.
        const auto field = nodeType->GetField(item->name);
        if (!field) {
            v->ReportError(ErrorCode::TYPE_UNKNOWN_FIELD, {item->name, nodeType->name}, item);
            return;
        }

        item->value->Accept(v);

        VisitorResult result;
        if (!v->TryGetResult(result)) return;

        auto type = TypeCoercion::coerce(result.value, field->type->ResolveType()->getLlvmType());
        if (type == nullptr) {
            v->ReportError(ErrorCode::UNKNOWN_ERROR, {}, item);
            return;
        }

        Compiler::getBuilder().CreateStore(
            type,
            Compiler::getBuilder().CreateStructGEP(nodeType->llvmType, context->var->alloc, field->index)
        );
    }

    v->AddSuccess(context->var->alloc);
}
