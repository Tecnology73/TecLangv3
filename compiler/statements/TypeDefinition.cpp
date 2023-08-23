#include "TypeDefinition.h"
#include "../../ast/Statements.h"
#include "../../ast/Literals.h"
#include "../../ast/TopLevel.h"
#include "../Compiler.h"
#include "../codegen/FunctionContext.h"

llvm::Function *generateDefaultConstructor(Visitor *v, TypeDefinition *type) {
    auto funcType = llvm::FunctionType::get(
        type->llvmType->getPointerTo(),
        {},
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

    // Allocate the memory for the struct.
    auto ptr = Compiler::getBuilder().CreateCall(
        Compiler::getScopeManager().getFunction("malloc")->llvmFunction,
        Compiler::getBuilder().getInt32(
            Compiler::getModule().getDataLayout().getTypeAllocSize(type->llvmType)
        )
    );

    // Set the default values for the fields.
    for (const auto &item: type->fields) {
        auto field = Compiler::getBuilder().CreateStructGEP(
            type->llvmType,
            ptr,
            type->getFieldIndex(item.first),
            item.second->name
        );

        llvm::Value *value;
        if (item.second->expression)
            value = item.second->expression->Accept(v);
        else
            value = item.second->type->getDefaultValue();

        Compiler::getBuilder().CreateStore(value, field);
    }

    // Register the constructor statement.
    auto function = new Function();
    function->name = "construct";
    function->ownerType = type;
    function->returnType = type;
    function->llvmFunction = func;

    type->addFunction(function);

    // Return instance
    Compiler::getBuilder().CreateRet(ptr);

    return func;
}

llvm::Function *generateConstructor(Visitor *visitor, Function *function, llvm::Function *defaultConstruct) {
    auto type = function->ownerType;

    // Compile a list of all the parameters.
    std::vector<llvm::Type *> params;
    for (const auto &item: function->parameters)
        params.push_back(item->type->llvmType);

    // Define the statement
    auto funcType = llvm::FunctionType::get(
        type->llvmType->getPointerTo(),
        params,
        false
    );
    auto func = function->llvmFunction = llvm::Function::Create(
        funcType,
        llvm::Function::ExternalLinkage,
        type->name + "_construct",
        Compiler::getModule()
    );

    auto entry = llvm::BasicBlock::Create(Compiler::getContext(), "entry", func);
    Compiler::getBuilder().SetInsertPoint(entry);

    // Create a scope
    Compiler::getScopeManager().enter("construct", new FunctionContext(visitor, function));

    // Call the default constructor which properly initializes everything.
    auto ptr = Compiler::getBuilder().CreateCall(defaultConstruct);

    // Make sure "this" is available.
    auto varDecl = new VariableDeclaration(function->token);
    varDecl->name = "this";
    varDecl->type = type;
    varDecl->alloc = ptr;
    Compiler::getScopeManager().add(varDecl);

    // Generate the parameters.
    for (const auto &item: function->parameters)
        item->Accept(visitor);

    // Generate the body.
    for (auto &statement: function->body)
        statement->Accept(visitor);

    // Return instance
    Compiler::getBuilder().CreateRet(ptr);

    // Cleanup
    Compiler::getScopeManager().popContext();
    Compiler::getScopeManager().leave("construct");
    return func;
}

void generateFunctions(Visitor *v, TypeDefinition *type) {
    auto lastBlock = Compiler::getBuilder().GetInsertBlock();

    // Generate the default constructor `func<T> construct() {}`
    auto defaultConstruct = generateDefaultConstructor(v, type);

    // Generate all the functions for the type
    for (const auto &item: type->functions)
        for (const auto &func: item.second) {
            if (func->llvmFunction) {
                // Already generated.
                continue;
            }

            if (item.first == "construct") {
                generateConstructor(v, func, defaultConstruct);
                continue;
            }

            // Normal statement.
            func->Accept(v);
        }

    // Restore the insert point.
    Compiler::getBuilder().SetInsertPoint(lastBlock);
}

TypeBase *inferType(Visitor *v, Node *n) {
    if (!n)
        return nullptr;

    if (auto pVariableReference = dynamic_cast<VariableReference *>(n)) {
        auto var = Compiler::getScopeManager().getVar(pVariableReference->name);
        if (!var)
            return nullptr;

        // Recurse through the chainable nodes and infer the types for each of them.
        auto node = (ChainableNode *) pVariableReference;
        auto type = node->ownerType = dynamic_cast<TypeDefinition *>(var->type);
        while (node->next) {
            node = node->next;
            type = node->ownerType = dynamic_cast<TypeDefinition *>(inferType(v, node));
            if (!type)
                return nullptr;
        }

        return type;
    } else if (auto pStaticRef = dynamic_cast<StaticRef *>(n)) {
        if (auto anEnum = Compiler::getScopeManager().getEnum(pStaticRef->name))
            return anEnum;

        return Compiler::getScopeManager().getType(pStaticRef->name);
    } else if (auto pVariableDeclaration = dynamic_cast<VariableDeclaration *>(n)) {
        if (pVariableDeclaration->type)
            return dynamic_cast<TypeDefinition *>( pVariableDeclaration->type);

        return inferType(v, pVariableDeclaration->expression);
    } else if (auto pFunctionCall = dynamic_cast<FunctionCall *>(n)) {
        Function *function;
        if (pFunctionCall->prev && pFunctionCall->prev->ownerType)
            // Lookup the statement on the type.
            function = pFunctionCall->prev->ownerType->getFunction(pFunctionCall->name);
        else
            // Lookup the global statement.
            function = Compiler::getScopeManager().getFunction(pFunctionCall->name);

        if (function)
            return inferType(v, function);
    } else if (auto pFunction = dynamic_cast<Function *>(n))
        return pFunction->returnType;
    else if (auto pBoolean = dynamic_cast<Boolean *>(n))
        return Compiler::getScopeManager().getType("bool");
    else if (auto pInteger = dynamic_cast<Integer *>(n)) {
        if (pInteger->numBits == 1)
            return Compiler::getScopeManager().getType("bool");

        return Compiler::getScopeManager().getType("i" + std::to_string(pInteger->numBits));
    } else if (auto pDouble = dynamic_cast<Double *>(n))
        return Compiler::getScopeManager().getType("double");
    else if (auto pString = dynamic_cast<String *>(n))
        return Compiler::getScopeManager().getType("string");
    else if (auto pConstructorCall = dynamic_cast<ConstructorCall *>(n))
        return pConstructorCall->type;
    else if (auto pWhen = dynamic_cast<When *>(n))
        // TODO: Infer the type of the when statement (somehow).
        return Compiler::getScopeManager().getType("i32");

    // FIXME: We reach this when trying to do `var = func()` and `func` doesn't exist.
    v->ReportError(ErrorCode::TYPE_INFER_FAILED, {n->token.value}, n);
    return nullptr;
}

llvm::Type *generateTypeDefinition(Visitor *v, TypeBase *t) {
    return generateTypeDefinition(v, dynamic_cast<TypeDefinition *>(t));
}

llvm::Type *generateTypeDefinition(Visitor *v, TypeDefinition *type) {
    if (type->llvmType)
        return type->llvmType;

    if (type->name == "i8")
        return type->setLlvmType(Compiler::getBuilder().getInt8Ty());
    else if (type->name == "i8*")
        return type->setLlvmType(Compiler::getBuilder().getInt8PtrTy());
    else if (type->name == "i16")
        return type->setLlvmType(Compiler::getBuilder().getInt16Ty());
    else if (type->name == "i32" || type->name == "int")
        return type->setLlvmType(Compiler::getBuilder().getInt32Ty());
    else if (type->name == "i64")
        return type->setLlvmType(Compiler::getBuilder().getInt64Ty());
    else if (type->name == "double")
        return type->setLlvmType(Compiler::getBuilder().getDoubleTy());
    else if (type->name == "bool")
        return type->setLlvmType(Compiler::getBuilder().getInt1Ty());
    else if (type->name == "void")
        return type->setLlvmType(Compiler::getBuilder().getVoidTy());

    // Generate the fields for the struct
    std::vector<llvm::Type *> fields;
    for (const auto &item: type->fields) {
        auto t = item.second->GetCompiledType(v)->llvmType;
        if (item.second->type->isStruct)
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
    generateFunctions(v, type);

    return type->llvmType;
}