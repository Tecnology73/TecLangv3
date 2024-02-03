#include "TypeFunction.h"
#include <llvm/IR/Function.h>

#include "TypeBase.h"
#include "../Compiler.h"
#include "../TypeCoercion.h"
#include "../../ast/StringInternTable.h"
#include "../../context/compiler/FunctionCompilerContext.h"

llvm::Function* compileDefaultConstructor(Visitor* visitor, TypeBase* type) {
    auto returnType = llvm::FunctionType::get(
        Compiler::getBuilder().getVoidTy(),
        {type->getLlvmType()},
        false
    );
    auto function = llvm::Function::Create(
        returnType,
        llvm::Function::PrivateLinkage,
        type->name + "_construct",
        Compiler::getModule()
    );

    auto entry = llvm::BasicBlock::Create(Compiler::getContext(), "entry", function);
    Compiler::getBuilder().SetInsertPoint(entry);

    // The instance is always passed in as the first argument.
    // This allows us to allocate memory for the instance
    // differently (stack vs heap) based on it's lifetime.
    auto ptr = function->getArg(0);
    ptr->setName("this");

    // Initialize all the fields.
    for (const auto& name: type->fieldOrder) {
        auto item = type->fields.at(name);
        auto field = Compiler::getBuilder().CreateStructGEP(
            type->llvmType,
            ptr,
            item->index,
            item->name
        );

        llvm::Value* value;
        if (item->expression) {
            item->expression->Accept(visitor);

            VisitorResult result;
            if (!visitor->TryGetResult(result)) return nullptr;

            value = result.value;
        } else
            value = TypeCompiler::getDefaultValue(item->type);

        Compiler::getBuilder().CreateStore(
            TypeCoercion::coerce(value, value->getType()),
            field
        );
    }

    // Constructors always return void
    Compiler::getBuilder().CreateRetVoid();

    // Register the constructor.
    auto cstr = new Function(StringInternTable::Intern("construct"));
    cstr->ownerType = type;
    cstr->returnType = type->CreateReference();
    cstr->llvmFunction = function;
    cstr->AddParameter(type->token, StringInternTable::Intern("this"), type->CreateReference());

    type->AddFunction(cstr);

    return function;
}

llvm::Function* compileConstructor(Visitor* visitor, Function* function, llvm::Function* defaultConstructor) {
    if (function->llvmFunction)
        return function->llvmFunction;

    // Generate the function signature.
    std::vector<llvm::Type *> params = {};
    params.reserve(function->parameters.size());
    for (const auto& param: function->parameters | std::views::values) {
        auto paramType = TypeCompiler::compile(param->type);
        if (param->name == "this")
            paramType = paramType->getPointerTo();

        params[param->index] = paramType;
    }

    auto returnType = llvm::FunctionType::get(
        Compiler::getBuilder().getVoidTy(),
        params,
        false
    );
    auto func = llvm::Function::Create(
        returnType,
        llvm::Function::PrivateLinkage,
        function->name + "_construct",
        Compiler::getModule()
    );

    auto entry = llvm::BasicBlock::Create(Compiler::getContext(), "entry", func);
    Compiler::getBuilder().SetInsertPoint(entry);

    Compiler::getScopeManager().enter("construct", new FunctionCompilerContext(visitor, function));

    // Call the default constructor.
    Compiler::getBuilder().CreateCall(defaultConstructor, {func->getArg(0)});

    // Generate params.
    for (const auto& param: function->parameters | std::views::values) {
        param->Accept(visitor);

        if (VisitorResult result; !visitor->TryGetResult(result))
            return nullptr;
    }

    // Generate the body.
    for (const auto& item: function->body) {
        item->Accept(visitor);

        if (VisitorResult result; !visitor->TryGetResult(result))
            return nullptr;
    }

    // Constructors never return a value. The first argument is always the instance.
    Compiler::getBuilder().CreateRetVoid();

    // Cleanup.
    Compiler::getScopeManager().popContext();
    Compiler::getScopeManager().leave("construct");

    // Only set this once everything has successfully compiled.
    return function->llvmFunction = func;
}

bool TypeFunctionCompiler::compile(Visitor* visitor, TypeBase* type) {
    // We need to remember where we're currently inserting IR at because we
    // compile types/functions on-demand.
    auto lastBlock = Compiler::getBuilder().GetInsertBlock();

    // Generate the default constructor `func<T> construct() {}`
    auto defaultConstructor = compileDefaultConstructor(visitor, type);
    if (!defaultConstructor)
        return false;

    // Generate all the functions for the type.
    // TODO: Don't do this... Functions should be compiled on-demand.
    for (const auto& functions: type->functions | std::views::values) {
        for (const auto& function: functions) {
            if (function->name == "construct") {
                if (!compileConstructor(visitor, function, defaultConstructor))
                    return false;

                continue;
            }

            function->Accept(visitor);
            if (VisitorResult result; !visitor->TryGetResult(result))
                return false;
        }
    }

    // Restore the insert point.
    Compiler::getBuilder().SetInsertPoint(lastBlock);

    return true;
}
