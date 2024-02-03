#pragma once

#include "../ast/Visitor.h"
#include "../symbolTable/SymbolTable.h"
#include "../ast/Literals.h"
// Top Level
#include "topLevel/Function.h"
// Statements
#include "statements/Return.h"
#include "statements/VariableDeclaration.h"
// Expressions
#include "expressions/FunctionCall.h"
#include "expressions/BinaryOperation.h"

class PreAnalysisVisitor : public Visitor {
public:
    explicit PreAnalysisVisitor(Parser* p) : Visitor("PreAnalysisVisitor", p) {
    }

    /*
     * Top level
    */

    void Visit(class TypeField* node) override {
    }

    void Visit(class TypeDefinition* node) override {
    }

    void Visit(class Enum* node) override {
    }

    void Visit(class EnumValue* node) override {
    }

    void Visit(class EnumConstructor* node) override {
    }

    void Visit(class EnumParameter* node) override {
    }

    void Visit(Function* node) override {
        analyseFunction(this, node);
    }

    void Visit(class FunctionParameter* node) override {
    }

    /*
     * Statements
     */

    void Visit(Return* node) override {
        analyseReturn(this, node);
    }

    void Visit(VariableDeclaration* node) override {
        analyseVariableDeclaration(this, node);
    }

    void Visit(class IfStatement* node) override {
    }

    void Visit(class ForLoop* node) override {
    }

    void Visit(class Continue* node) override {
    }

    void Visit(class Break* node) override {
    }

    /*
     * Expressions
    */

    void Visit(class ConstructorCall* node) override {
    }

    void Visit(FunctionCall* node) override {
        analyseFunctionCall(this, node);
    }

    void Visit(BinaryOperation* node) override {
        analyseBinaryOperation(this, node);
    }

    void Visit(TypeReference* node) override {
    }

    void Visit(class VariableReference* node) override {
    }

    void Visit(class StaticRef* node) override {
    }

    void Visit(class When* node) override {
    }

    void Visit(class WhenCondition* node) override {
    }

    /*
     * Literals
     */

    void Visit(Integer* node) override {
        AddSuccess(node->getType());
    }

    void Visit(Double* node) override {
        AddSuccess(node->getType());
    }

    void Visit(Boolean* node) override {
        AddSuccess(node->getType());
    }

    void Visit(Null* node) override {
        AddSuccess(node->getType());
    }

    void Visit(String* node) override {
        AddSuccess(node->getType());
    }
};
