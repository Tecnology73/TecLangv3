#pragma once

#include <iostream>
#include "../ast/Visitor.h"
#include "../ast/TopLevel.h"
#include "../ast/Expressions.h"
#include "../ast/Statements.h"
#include "../ast/Literals.h"
#include "../Console.h"

class PrettyPrintVisitor : public Visitor {
public:
    int indent = 0;

    explicit PrettyPrintVisitor(Parser *p) : Visitor("PrettyPrintVisitor", p) {}

    template<typename... Args>
    inline void Print(const char *format, Args... args) const {
        // AddField a pipe every other indentation level.
        for (int i = 0; i < indent / 2; i++) {
            if (i % 2 == 1) std::cout << "  ";
            else std::cout << "|  ";
        }

        printInfo(format, args...);
    }

    /*
     * Top level
     */

    llvm::Value *Visit(class TypeField *node) override {
        Print("Type Field: %s\n", node->name.c_str());
        indent += 2;

        if (node->type != nullptr)
            Print("Type: %s\n", node->type->name.c_str());
        else
            Print("Type: infer\n");

        Print("Expression:\n");
        indent += 2;

        if (node->expression != nullptr)
            node->expression->Accept(this);
        else
            Print("default\n");

        indent -= 4;
        return nullptr;
    }

    llvm::Value *Visit(TypeDefinition *node) override {
        Print("Type: %s\n", node->name.c_str());
        indent += 2;

        Print("Fields: %d\n", node->fields.size());
        indent += 2;
        for (const auto &fieldName: node->fieldOrder)
            node->fields[fieldName]->Accept(this);

        indent -= 2;
        Print("Functions: %d\n", node->functions.size());
        indent += 2;
        for (auto &function: node->functions) {
            for (const auto &item: function.second)
                item->Accept(this);
        }

        indent -= 4;
        return nullptr;
    }

    llvm::Value *Visit(Enum *node) override {
        Print("Enum: %s\n", node->name.c_str());
        indent += 2;

        Print("Values: %d\n", node->fields.size());
        indent += 2;
        for (auto &fieldName: node->fieldOrder)
            node->fields[fieldName]->Accept(this);

        indent -= 4;
        return nullptr;
    }

    llvm::Value *Visit(EnumValue *node) override {
        Print("EnumValue: %s\n", node->name.c_str());
        indent += 2;

        if (node->expression != nullptr)
            node->expression->Accept(this);
        else
            Print("default\n");

        indent -= 2;
        return nullptr;
    }

    llvm::Value *Visit(EnumConstructor *node) override {
        Print("EnumConstructor: %s\n", node->name.c_str());
        indent += 2;

        for (auto &parameter: node->parameters)
            parameter->Accept(this);

        indent -= 2;
        return nullptr;
    }

    llvm::Value *Visit(EnumParameter *node) override {
        Print("%s: %s\n", node->name.c_str(), node->type->name.c_str());
        return nullptr;
    }

    llvm::Value *Visit(Function *node) override {
        Print("Function: %s\n", node->name.c_str());
        indent += 2;

        if (node->returnType)
            Print("Return type: %s\n", node->returnType->name.c_str());
        else
            Print("Return type: infer\n");

        Print("Parameters: %d\n", node->parameters.size());
        indent += 2;
        for (auto &paramName: node->parameterOrder)
            node->parameters[paramName]->Accept(this);

        indent -= 2;
        Print("Body: %d\n", node->body.size());
        indent += 2;
        for (auto &statement: node->body) {
            statement->Accept(this);
        }

        indent -= 4;
        return nullptr;
    }

    llvm::Value *Visit(FunctionParameter *node) override {
        Print("%s: %s\n", node->name.c_str(), node->type->name.c_str());
        return nullptr;
    }

    /*
     * Statements
     */

    llvm::Value *Visit(Return *node) override {
        Print("Return:\n");
        indent += 2;

        if (node->expression != nullptr)
            node->expression->Accept(this);
        else
            Print("default\n");

        indent -= 2;
        return nullptr;
    }

    llvm::Value *Visit(VariableDeclaration *node) override {
        Print("Variable declaration: %s\n", node->name.c_str());
        indent += 2;

        if (node->type != nullptr)
            Print("Type: %s\n", node->type->name.c_str());
        else
            Print("Type: infer\n");

        Print("Expression:\n");
        indent += 2;

        if (node->expression != nullptr)
            node->expression->Accept(this);
        else
            Print("default\n");

        indent -= 4;
        return nullptr;
    }

    llvm::Value *Visit(IfStatement *node) override {
        if (node->condition != nullptr) {
            Print("If:\n");
            indent += 2;

            Print("Condition:\n");
            indent += 2;
            node->condition->Accept(this);
            indent -= 4;
        }

        Print("Then:\n");
        indent += 2;
        for (auto &statement: node->body) {
            statement->Accept(this);
        }
        indent -= 2;

        if (node->elseStatement) {
            Print("Else:\n");
            indent += 2;
            node->elseStatement->Accept(this);
            indent -= 2;
        }
        return nullptr;
    }

    llvm::Value *Visit(ForLoop *node) override {
        Print("For:\n");
        indent += 2;

        Print("Head:\n");
        indent += 2;
        node->value->Accept(this);
        indent -= 2;

        Print("Step:\n");
        indent += 2;
        if (node->step)
            node->step->Accept(this);
        else
            Print(" default\n");
        indent -= 2;

        Print("As:\n");
        indent += 2;
        if (node->identifier)
            node->identifier->Accept(this);
        else
            Print(" default\n");
        indent -= 2;

        Print("Body:\n");
        indent += 2;
        for (auto &statement: node->body)
            statement->Accept(this);
        indent -= 2;

        indent -= 2;
        return nullptr;
    }

    llvm::Value *Visit(Continue *node) override {
        Print("Continue\n");
        return nullptr;
    }

    llvm::Value *Visit(Break *node) override {
        Print("Break\n");
        return nullptr;
    }

    /*
     * Expressions
     */

    llvm::Value *Visit(ConstructorCall *node) override {
        Print("Constructor call: %s\n", node->type->name.c_str());
        indent += 2;

        Print("Arguments: %d\n", node->arguments.size());
        indent += 2;
        for (auto &argument: node->arguments) {
            argument->Accept(this);
        }

        indent -= 4;
        return nullptr;
    }

    llvm::Value *Visit(FunctionCall *node) override {
        Print("Function call: %s\n", node->name.c_str());
        indent += 2;

        Print("Arguments: %d\n", node->arguments.size());
        indent += 2;
        for (auto &argument: node->arguments)
            argument->Accept(this);
        indent -= 2;

        if (node->next)
            node->next->Accept(this);

        indent -= 2;
        return nullptr;
    }

    llvm::Value *Visit(BinaryOperation *node) override {
        Print("Binary operation: %d\n", node->op);
        indent += 2;

        Print("Left:\n");
        indent += 2;
        node->lhs->Accept(this);
        indent -= 2;

        Print("Right:\n");
        indent += 2;
        node->rhs->Accept(this);

        indent -= 4;
        return nullptr;
    }

    llvm::Value *Visit(VariableReference *node) override {
        Print("VarRef: %s\n", node->name.c_str());
        indent += 2;
        if (node->next)
            node->next->Accept(this);

        indent -= 2;
        return nullptr;
    }

    llvm::Value *Visit(StaticRef *node) override {
        Print("StaticRef: %s\n", node->name.c_str());
        indent += 2;
        if (node->next)
            node->next->Accept(this);

        indent -= 2;
        return nullptr;
    }

    llvm::Value *Visit(When *node) override {
        Print("When:\n");
        indent += 2;

        Print("Expression:\n");
        indent += 2;
        node->expression->Accept(this);
        indent -= 2;

        Print("Cases: %d\n", node->body.size());
        indent += 2;
        for (auto &item: node->body)
            item->Accept(this);
        indent -= 2;

        indent -= 2;
        return nullptr;
    }

    llvm::Value *Visit(WhenCondition *node) override {
        Print("WhenCondition:\n");
        indent += 2;

        if (node->condition != nullptr) {
            Print("Condition:\n");
            indent += 2;
            node->condition->Accept(this);
            indent -= 2;
        } else
            Print("Else:\n");

        Print("Body: %d\n", node->body.size());
        indent += 2;
        for (auto &statement: node->body)
            statement->Accept(this);
        indent -= 2;

        indent -= 2;
        return nullptr;
    }

    /*
     * Literals
     */

    llvm::Value *Visit(Integer *node) override {
        Print("Integer: %d\n", node->value);
        return nullptr;
    }

    llvm::Value *Visit(Double *node) override {
        Print("Double: %f\n", node->value);
        return nullptr;
    }

    llvm::Value *Visit(Boolean *node) override {
        Print("Boolean: %d\n", node->value);
        return nullptr;
    }

    llvm::Value *Visit(String *node) override {
        Print("String: %s\n", node->value.c_str());
        return nullptr;
    }
};
