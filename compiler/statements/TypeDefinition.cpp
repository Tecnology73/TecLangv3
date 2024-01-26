#include "TypeDefinition.h"
#include "../../ast/Expressions.h"
#include "../../ast/Statements.h"
#include "../../ast/Literals.h"
#include "../../ast/TopLevel.h"
#include "../Compiler.h"
#include "../TypeCoercion.h"
#include "../../context/compiler/FunctionCompilerContext.h"
#include "../../ast/StringInternTable.h"

llvm::Function* generateDefaultConstructor(Visitor* v, TypeDefinition* type) {
    auto funcType = llvm::FunctionType::get(
        Compiler::getBuilder().getVoidTy(),
        {
            type->getLlvmType()->getPointerTo()
        },
        false
    );
    auto func = llvm::Function::Create(
        funcType,
        llvm::Function::ExternalLinkage,
        type->name + "_construct",
        Compiler::getModule()
    );

    auto entry = llvm::BasicBlock::Create(Compiler::getContext(), "entry", func);
    Compiler::getBuilder().SetInsertPoint(entry);

    // Instance is passed in.
    // This allows us to allocate memory for the instance
    // differently (stack vs heap) based on it's lifetime.
    auto ptr = func->getArg(0);
    ptr->setName("this");

    // Set the default values for the fields.
    for (const auto& fieldName: type->fieldOrder) {
        auto item = type->fields[fieldName];
        auto field = Compiler::getBuilder().CreateStructGEP(
            type->llvmType,
            ptr,
            item->index,
            item->name
        );

        llvm::Value* value;
        if (item->expression) {
            item->expression->Accept(v);

            VisitorResult result;
            if (!v->TryGetResult(result)) return nullptr;

            value = result.value;
        } else if (type->name == "string" && fieldName == "data") {
            auto emptyStr = Compiler::getBuilder().CreateGlobalStringPtr("\0");
            value = Compiler::getBuilder().CreateCall(
                SymbolTable::GetInstance()->LookupFunction("malloc", {})->llvmFunction,
                {
                    Compiler::getBuilder().getInt64(1)
                }
            );

            Compiler::getBuilder().CreateCall(
                SymbolTable::GetInstance()->LookupFunction("strcpy", {})->llvmFunction,
                {
                    value,
                    emptyStr
                }
            );
        } else
            value = item->type->ResolveType()->getDefaultValue();

        Compiler::getBuilder().CreateStore(
            TypeCoercion::coerce(value, item->type->ResolveType()->getLlvmType()),
            field
        );
    }

    Compiler::getBuilder().CreateRetVoid();

    // Register the constructor statement.
    auto function = new Function(StringInternTable::Intern("construct"));
    function->ownerType = type;
    function->returnType = type->CreateReference();
    function->llvmFunction = func;
    function->AddParameter(type->token, StringInternTable::Intern("this"), type->CreateReference());

    type->AddFunction(function);

    return func;
}

llvm::Function* generateConstructor(Visitor* visitor, Function* function, llvm::Function* defaultConstruct) {
    auto variant = function->ownerType;

    // Compile a list of all the parameters.
    std::vector<llvm::Type *> params = {};
    for (const auto& paramName: function->parameterOrder) {
        auto ty = function->parameters[paramName]->type->ResolveType()->llvmType;
        if (paramName == "this")
            ty = ty->getPointerTo();
        params.push_back(ty);
    }

    // Define the statement
    auto funcType = llvm::FunctionType::get(
        Compiler::getBuilder().getVoidTy(),
        params,
        false
    );
    auto func = function->llvmFunction = llvm::Function::Create(
                    funcType,
                    llvm::Function::ExternalLinkage,
                    variant->name + "_construct",
                    Compiler::getModule()
                );

    auto entry = llvm::BasicBlock::Create(Compiler::getContext(), "entry", func);
    Compiler::getBuilder().SetInsertPoint(entry);

    // Create a scope
    Compiler::getScopeManager().enter("construct", new FunctionCompilerContext(visitor, function));

    // Call the default constructor which properly initializes everything.
    Compiler::getBuilder().CreateCall(defaultConstruct, {func->getArg(0)});

    // Generate the parameters.
    for (const auto& paramName: function->parameterOrder) {
        function->parameters[paramName]->Accept(visitor);
        if (!visitor->GetResult().success)
            return nullptr;
    }

    // Generate the body.
    for (auto& statement: function->body) {
        statement->Accept(visitor);
        if (!visitor->GetResult().success)
            return nullptr;
    }

    // Return instance
    Compiler::getBuilder().CreateRetVoid();

    // Cleanup
    Compiler::getScopeManager().popContext();
    Compiler::getScopeManager().leave("construct");
    return func;
}

bool generateFunctions(Visitor* v, TypeDefinition* type) {
    auto lastBlock = Compiler::getBuilder().GetInsertBlock();

    // Generate the default constructor `func<T> construct() {}`
    auto defaultConstruct = generateDefaultConstructor(v, type);
    if (!defaultConstruct) {
        v->AddFailure();
        return false;
    }

    // Generate all the functions for the type
    for (const auto& item: type->functions)
        for (const auto& func: item.second) {
            if (func->llvmFunction) {
                // Already generated.
                continue;
            }

            if (item.first == "construct") {
                if (!generateConstructor(v, func, defaultConstruct)) {
                    v->AddFailure();
                    return false;
                }

                continue;
            }

            // Normal statement.
            func->Accept(v);

            if (VisitorResult result; !v->TryGetResult(result))
                return false;
        }

    // Restore the insert point.
    Compiler::getBuilder().SetInsertPoint(lastBlock);

    return true;
}

TypeReference* inferType(Visitor* v, Node* node) {
    if (!node)
        return nullptr;

    // TODO: Would be nice to replace the VarRef/StaticRef/FunctionCall with this.
    /*if (auto chainNode = dynamic_cast<ChainableNode*>(node))
        return chainNode->getFinalType();*/

    if (auto varRef = dynamic_cast<VariableReference *>(node))
        return varRef->type;

    if (auto pStaticRef = dynamic_cast<StaticRef *>(node))
        return SymbolTable::GetInstance()->GetReference(pStaticRef->name);

    if (auto funcCall = dynamic_cast<FunctionCall *>(node))
        return funcCall->function->returnType;

    if (auto pVariableDeclaration = dynamic_cast<VariableDeclaration *>(node)) {
        /*if (pVariableDeclaration->type)
            return pVariableDeclaration->type->ResolveType();

        return inferType(v, pVariableDeclaration->expression);*/
        return pVariableDeclaration->type;
    }

    if (auto function = dynamic_cast<Function *>(node))
        return function->returnType;

    if (auto literal = dynamic_cast<Literal *>(node))
        return literal->getType();

    if (auto constructCall = dynamic_cast<ConstructorCall *>(node))
        return constructCall->type;

    if (auto when = dynamic_cast<When *>(node))
        return when->returnType;

    // FIXME: We reach this when trying to do `var = func()` and `func` doesn't exist.
    v->ReportError(ErrorCode::TYPE_INFER_FAILED, {node->token.value.data()}, node);
    return nullptr;
}

llvm::Type* generateTypeDefinition(Visitor* v, TypeBase* t) {
    if (auto typeDef = dynamic_cast<TypeDefinition *>(t))
        return generateTypeDefinition(v, typeDef);

    return t->getLlvmType();
}

llvm::Type* generateTypeDefinition(Visitor* v, TypeDefinition* type) {
    if (type->llvmType)
        return type->getLlvmType();

    if (type->name == "i8" || type->name == "u8")
        return type->setLlvmType(Compiler::getBuilder().getInt8Ty());
    if (type->name == "i16" || type->name == "u16")
        return type->setLlvmType(Compiler::getBuilder().getInt16Ty());
    if (type->name == "i32" || type->name == "u32")
        return type->setLlvmType(Compiler::getBuilder().getInt32Ty());
    if (type->name == "i64" || type->name == "u64")
        return type->setLlvmType(Compiler::getBuilder().getInt64Ty());
    if (type->name == "double")
        return type->setLlvmType(Compiler::getBuilder().getDoubleTy());
    if (type->name == "bool")
        return type->setLlvmType(Compiler::getBuilder().getInt1Ty());
    if (type->name == "void")
        return type->setLlvmType(Compiler::getBuilder().getVoidTy());
    if (type->name == "ptr")
        return type->setLlvmType(Compiler::getBuilder().getPtrTy());

    // Generate the fields for the struct
    std::vector<llvm::Type *> fields;
    for (const auto& fieldName: type->fieldOrder) {
        auto field = type->fields[fieldName];
        if (!field->type->ResolveType()->llvmType) {
            if (auto typeDef = dynamic_cast<TypeDefinition *>(field->type->ResolveType()))
                generateTypeDefinition(v, typeDef);
        }

        auto t = field->type->ResolveType()->getLlvmType();
        if (!field->type->ResolveType()->isValueType)
            t = t->getPointerTo();

        fields.push_back(t);
    }

    // Set the compiled type.
    type->setLlvmType(
        llvm::StructType::create(
            Compiler::getContext(),
            fields,
            type->name
        )
    );

    // Generate all the functions for the type.
    // This includes the constructors.
    if (!generateFunctions(v, type))
        return nullptr;

    return type->llvmType;
}
