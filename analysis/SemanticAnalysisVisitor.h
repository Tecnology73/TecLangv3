#pragma once

#include "../ast/Visitor.h"
//
#include "topLevel/Enum.h"
#include "topLevel/Function.h"
//
#include "expressions/When.h"

class SemanticAnalysisVisitor : public Visitor {
public:
    explicit SemanticAnalysisVisitor(Parser *p) : Visitor("SemanticAnalysisVisitor", p) {}

    /*
     * Top level
     */

    llvm::Value *Visit(class TypeField *node) override { return nullptr; }

    llvm::Value *Visit(class TypeDefinition *node) override { return nullptr; }

    llvm::Value *Visit(class Enum *node) override {
        EnumAnalyzer(this, node).Analyze();
        return nullptr;
    }

    llvm::Value *Visit(class EnumValue *node) override { return nullptr; }

    llvm::Value *Visit(class EnumConstructor *node) override { return nullptr; }

    llvm::Value *Visit(class EnumParameter *node) override { return nullptr; }

    llvm::Value *Visit(class Function *node) override {
        FunctionAnalyzer(this, node).Analyze();
        return nullptr;
    }

    llvm::Value *Visit(class FunctionParameter *node) override { return nullptr; }

    /*
     * Statements
     */

    llvm::Value *Visit(class Return *node) override { return nullptr; }

    llvm::Value *Visit(class VariableDeclaration *node) override { return nullptr; }

    llvm::Value *Visit(class IfStatement *node) override { return nullptr; }

    llvm::Value *Visit(class ForLoop *node) override { return nullptr; }

    llvm::Value *Visit(class Continue *node) override { return nullptr; }

    llvm::Value *Visit(class Break *node) override { return nullptr; }

    /*
     * Expressions
     */

    llvm::Value *Visit(class ConstructorCall *node) override { return nullptr; }

    llvm::Value *Visit(class FunctionCall *node) override { return nullptr; }

    llvm::Value *Visit(class BinaryOperation *node) override { return nullptr; }

    llvm::Value *Visit(class VariableReference *node) override { return nullptr; }

    llvm::Value *Visit(class StaticRef *node) override { return nullptr; }

    llvm::Value *Visit(class When *node) override {
        WhenAnalyzer(this, node).Analyze();
        return nullptr;
    }

    llvm::Value *Visit(class WhenCondition *node) override { return nullptr; }

    /*
     * Literals
     */

    llvm::Value *Visit(class Integer *node) override { return nullptr; }

    llvm::Value *Visit(class Double *node) override { return nullptr; }

    llvm::Value *Visit(class Boolean *node) override { return nullptr; }

    llvm::Value *Visit(class String *node) override { return nullptr; }
};
