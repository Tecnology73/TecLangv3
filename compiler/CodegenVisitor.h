#pragma once

#include "Compiler.h"
#include "../ast/TopLevel.h"
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
#include "statements/Function.h"
#include "statements/Return.h"
#include "statements/IfStatement.h"
#include "statements/VariableDeclaration.h"
#include "statements/ForLoop.h"
#include "statements/Continue.h"
#include "statements/Break.h"

class CodegenVisitor : public Visitor {
public:
    explicit CodegenVisitor(Parser* p) : Visitor("CodegenVisitor", p) {
        Compiler::getInstance();
    }

    /*
     * Top-level
     */

    void Visit(class TypeField* node) override {
    }

    void Visit(TypeDefinition* node) override {
        // Types are generated on an as-needed basis.
    }

    void Visit(Enum* node) override {
        generateEnum(this, node);
    }

    void Visit(EnumValue* node) override {
    }

    void Visit(EnumConstructor* node) override {
    }

    void Visit(EnumParameter* node) override {
    }

    void Visit(Function* node) override {
        generateFunction(this, node);
    }

    void Visit(FunctionParameter* node) override {
        generateFunctionParameter(this, node);
    }

    /*
     * Statements
     */

    void Visit(Return* node) override {
        generateReturn(this, node);
    }

    void Visit(VariableDeclaration* node) override {
        generateVariableDeclaration(this, node);
    }

    void Visit(IfStatement* node) override {
        generateIfStatement(this, node);
    }

    void Visit(ForLoop* node) override {
        generateForLoop(this, node);
    }

    void Visit(Continue* node) override {
        generateContinue(this, node);
    }

    void Visit(Break* node) override {
        generateBreak(this, node);
    }

    /*
     * Expressions
     */

    void Visit(ConstructorCall* node) override {
        generateConstructorCall(this, node);
    }

    void Visit(FunctionCall* node) override {
        generateFunctionCall(this, node);
    }

    void Visit(BinaryOperation* node) override {
        generateBinaryOperation(this, node);
    }

    void Visit(TypeReference* node) override {
    }

    void Visit(VariableReference* node) override {
        generateVariableReference(this, node);
    }

    void Visit(ArrayRef* node) override {
    }

    void Visit(StaticRef* node) override {
        generateStaticRef(this, node);
    }

    void Visit(When* node) override {
        generateWhen(this, node);
    }

    void Visit(WhenCondition* node) override {
        // The generation for this is handled by the When expression.
    }

    /*
     * Literals
     */

    void Visit(Integer* node) override {
        AddSuccess(Compiler::getBuilder().getIntN(node->numBits, node->value), node->getType());
    }

    void Visit(Double* node) override {
        AddSuccess(llvm::ConstantFP::get(Compiler::getBuilder().getDoubleTy(), node->value), node->getType());
    }

    void Visit(Boolean* node) override {
        AddSuccess(Compiler::getBuilder().getInt1(node->value), node->getType());
    }

    void Visit(String* node) override {
        auto zero = Compiler::getBuilder().getInt32(0);
        node->llvmValue = Compiler::getBuilder().CreateGlobalString(node->value);

        auto value = Compiler::getBuilder().CreateInBoundsGEP(
            node->llvmValue->getValueType(),
            node->llvmValue,
            {zero, zero}
        );

        AddSuccess(value, node->getType());
    }

    void Visit(Null* node) override {
        AddSuccess(llvm::ConstantPointerNull::get(Compiler::getBuilder().getPtrTy()));
    }
};
