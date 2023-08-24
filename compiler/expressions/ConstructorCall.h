#pragma once

#include <llvm/IR/Value.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Verifier.h>
#include <string>
#include <format>
#include "../../ast/Expressions.h"
#include "../Compiler.h"
#include "../statements/TypeDefinition.h"
#include "../ErrorManager.h"
#include "../TypeCoercion.h"

llvm::Value *generateConstructorCall(Visitor *v, ConstructorCall *node) {
    generateTypeDefinition(v, node->type);

    // FIXME: This could probably be a little cleaner/efficient if we just pass in `node->parameters`
    //  to the statement lookup and (somehow) get the TypeDefinition from the node.
    // This would allow us to do any type coercion (or just override the bits for an integer?).
    // Perhaps we could just have a `TypeDefinition::callFunction` that does all the work for us?
    std::vector<llvm::Value *> args;
    for (const auto &item: node->arguments)
        args.push_back(item->Accept(v));

    // Find the constructor that matches the argTypes.
    /*auto constructFunction = node->type->getFunction("construct", args);
    if (!constructFunction) {
        std::string argTypes;
        for (const auto &item: args) {
            argTypes += Compiler::getScopeManager().getType(item->getType())->name;
            if (&item != &args.back()) argTypes += ", ";
        }

        v->ReportError(ErrorCode::TYPE_UNKNOWN_CONSTRUCTOR, {node->type->name, argTypes}, node);
        return nullptr;
    }

    // Make sure that the parameters are all the same type as the statement parameters so LLVM doesn't complain.
    for (int i = 0; i < args.size(); i++)
        args[i] = TypeCoercion::coerce(args[i], constructFunction->parameters[i]->type->llvmType);

    // Call the constructor
    auto ptr = Compiler::getBuilder().CreateCall(constructFunction->llvmFunction, args);

    // If this is actually "assignment constructor", do all the assignments.
    // a = new T { x = 1, y = 2 }
    for (const auto &item: node->assignments) {
        // Make sure the field exists on the type.
        auto fieldIndex = node->type->getFieldIndex(item->name);
        if (fieldIndex == -1) {
            v->ReportError(ErrorCode::TYPE_UNKNOWN_FIELD, {item->name, node->type->name}, item);
            return nullptr;
        }

        Compiler::getBuilder().CreateStore(
            item->expression->Accept(v),
            Compiler::getBuilder().CreateStructGEP(node->type->llvmType, ptr, fieldIndex)
        );
    }

    return ptr;*/
    return nullptr;
}