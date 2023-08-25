#pragma once

#include "Compiler.h"
#include "../ast/TopLevel.h"
#include "../ast/Statements.h"
#include "../ast/Expressions.h"
#include "../ast/Literals.h"
// Top-level
#include "topLevel/Enum.h"
// Expressions
#include "expressions/VariableReference.h"
#include "expressions/FunctionArgument.h"
#include "expressions/FunctionCall.h"
#include "expressions/BinaryOperation.h"
#include "expressions/ConstructorCall.h"
#include "expressions/StaticRef.h"
#include "expressions/When.h"
// Statements
#include "statements/TypeDefinition.h"
#include "statements/Function.h"
#include "statements/Return.h"
#include "statements/IfStatement.h"
#include "statements/VariableDeclaration.h"
#include "statements/ForLoop.h"
#include "statements/Continue.h"
#include "statements/Break.h"

class CodegenVisitor : public Visitor {
public:
    explicit CodegenVisitor(Parser *p) : Visitor("CodegenVisitor", p) {
        Compiler::getInstance();
    }

    /*
     * Top-level
     */

    llvm::Value *Visit(class TypeField *node) override {
        return nullptr;
    }

    llvm::Value *Visit(TypeDefinition *node) override {
        // Types are generated on an as-needed basis.
        return nullptr;
    }

    llvm::Value *Visit(Enum *node) override {
        return generateEnum(this, node);
    }

    llvm::Value *Visit(EnumValue *node) override {
        return nullptr;
    }

    llvm::Value *Visit(EnumConstructor *node) override {
        return nullptr;
    }

    llvm::Value *Visit(EnumParameter *node) override {
        return nullptr;
    }

    llvm::Value *Visit(Function *node) override {
        return generateFunction(this, node);
    }

    llvm::Value *Visit(FunctionParameter *node) override {
        return generateFunctionParameter(this, node);
    }

    /*
     * Statements
     */

    llvm::Value *Visit(Return *node) override {
        return generateReturn(this, node);
    }

    llvm::Value *Visit(VariableDeclaration *node) override {
        return generateVariableDeclaration(this, node);
    }

    llvm::Value *Visit(IfStatement *node) override {
        return generateIfStatement(this, node);
    }

    llvm::Value *Visit(ForLoop *node) override {
        return generateForLoop(this, node);
    }

    llvm::Value *Visit(Continue *node) override {
        return generateContinue(this, node);
    }

    llvm::Value *Visit(Break *node) override {
        return generateBreak(this, node);
    }

    /*
     * Expressions
     */

    llvm::Value *Visit(ConstructorCall *node) override {
        return generateConstructorCall(this, node);
    }

    llvm::Value *Visit(FunctionCall *node) override {
        return generateFunctionCall(this, node);
    }

    llvm::Value *Visit(BinaryOperation *node) override {
        return generateBinaryOperation(this, node);
    }

    llvm::Value *Visit(VariableReference *node) override {
        return generateVariableReference(this, node);
    }

    llvm::Value *Visit(StaticRef *node) override {
        return generateStaticRef(this, node);
    }

    llvm::Value *Visit(When *node) override {
        return generateWhen(this, node);
    }

    llvm::Value *Visit(WhenCondition *node) override {
        // The generation for this is handled by the When expression.
        return nullptr;
    }

    /*
     * Literals
     */

    llvm::Value *Visit(Integer *node) override {
        return Compiler::getBuilder().getIntN(node->numBits, node->value);
    }

    llvm::Value *Visit(Double *node) override {
        return llvm::ConstantFP::get(Compiler::getBuilder().getDoubleTy(), node->value);
    }

    llvm::Value *Visit(Boolean *node) override {
        return Compiler::getBuilder().getInt1(node->value);
    }

    llvm::Value *Visit(String *node) override {
        return Compiler::getBuilder().CreateGlobalStringPtr(node->value);
    }
};

