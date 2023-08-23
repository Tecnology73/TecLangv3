#pragma once

#include <iostream>
#include <llvm/IR/Value.h>
#include "../parser/Parser.h"
#include "../compiler/ErrorManager.h"

class Visitor {
public:
    std::string name;

    Visitor(std::string n, Parser *p) : name(std::move(n)), parser(p) {}

    void ReportError(
        ErrorCode errorCode,
        const std::vector<std::string> &args,
        const Node *node
    ) const {
        ErrorManager::Report(errorCode, args, parser, node);
    }

    /*
     * Expressions
     */

    virtual llvm::Value *Visit(class ConstructorCall *node) = 0;

    virtual llvm::Value *Visit(class FunctionParameter *node) = 0;

    virtual llvm::Value *Visit(class FunctionCall *node) = 0;

    virtual llvm::Value *Visit(class BinaryOperation *node) = 0;

    virtual llvm::Value *Visit(class VariableReference *node) = 0;

    virtual llvm::Value *Visit(class StaticRef *node) = 0;

    virtual llvm::Value *Visit(class When *node) = 0;

    virtual llvm::Value *Visit(class WhenCondition *node) = 0;

    /*
     * Statements
     */

    virtual void Visit(class TypeDefinition *node) = 0;

    virtual llvm::Value *Visit(class Function *node) = 0;

    virtual llvm::Value *Visit(class Enum *node) = 0;

    virtual llvm::Value *Visit(class EnumValue *node) = 0;

    virtual llvm::Value *Visit(class EnumConstructor *node) = 0;

    virtual llvm::Value *Visit(class EnumParameter *node) = 0;

    virtual llvm::Value *Visit(class Return *node) = 0;

    virtual llvm::Value *Visit(class VariableDeclaration *node) = 0;

    virtual llvm::Value *Visit(class IfStatement *node) = 0;

    virtual llvm::Value *Visit(class ForLoop *node) = 0;

    virtual llvm::Value *Visit(class Continue *node) = 0;

    virtual llvm::Value *Visit(class Break *node) = 0;

    /*
     * Literals
     */

    virtual llvm::Value *Visit(class Integer *node) = 0;

    virtual llvm::Value *Visit(class Double *node) = 0;

    virtual llvm::Value *Visit(class Boolean *node) = 0;

    virtual llvm::Value *Visit(class String *node) = 0;

private:
    Parser *parser;
};

