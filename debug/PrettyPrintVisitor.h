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

    explicit PrettyPrintVisitor(Parser* p) : Visitor("PrettyPrintVisitor", p) {
    }

    template<typename... Args>
    inline void Print(const char* format, Args... args) const {
        // Add a pipe every other indentation level.
        for (int i = 0; i < indent / 2; i++) {
            if (i % 2 == 1) std::cout << "  ";
            else std::cout << "|  ";
        }

        printInfo(format, args...);
    }

    std::string FormatType(const TypeReference* type) const {
        if (type == nullptr)
            return "infer";

        if (type->flags.Has(TypeFlag::OPTIONAL))
            return std::format("Option<{}>", type->name);

        return type->name;
    }

    std::string FormatTypeRef(const TypeReference* type) const {
        if (type == nullptr)
            return "infer";

        if (type->flags.Has(TypeFlag::OPTIONAL))
            return std::format("Option<{}>", type->name);

        return type->name;
    }

    void PrintTypeFlags(const BitFlag<TypeFlag>& flags) {
        Print("Flags:\n");
        indent += 2;

        if (flags.Has(TypeFlag::PUBLIC))Print("Access: Public\n");
        else if (flags.Has(TypeFlag::PRIVATE)) Print("Access: Private\n");
        else Print("Access: Inherit\n");

        Print("Optional: %s\n", flags.Has(TypeFlag::OPTIONAL) ? "Yes" : "No");

        indent -= 2;
    }

    /*
     * Top level
     */

    void Visit(class TypeField* node) override {
        Print("Type Field: %s\n", node->name.c_str());
        indent += 2;

        Print("Type: %s\n", FormatTypeRef(node->type).c_str());
        PrintTypeFlags(node->flags);

        Print("Expression:\n");
        indent += 2;

        if (node->expression != nullptr)
            node->expression->Accept(this);
        else
            Print("default\n");

        indent -= 4;
    }

    void Visit(TypeDefinition* node) override {
        Print("Type: %s\n", node->name.c_str());
        indent += 2;

        PrintTypeFlags(node->flags);

        Print("Fields: %d\n", node->fields.size());
        indent += 2;
        for (const auto& fieldName: node->fieldOrder)
            node->fields[fieldName]->Accept(this);

        indent -= 2;
        Print("Functions: %d\n", node->functions.size());
        indent += 2;
        for (auto& function: node->functions) {
            for (const auto& item: function.second)
                item->Accept(this);
        }

        indent -= 4;
    }

    void Visit(Enum* node) override {
        Print("Enum: %s\n", node->name.c_str());
        indent += 2;

        Print("Values: %d\n", node->fields.size());
        indent += 2;
        for (auto& fieldName: node->fieldOrder)
            node->fields[fieldName]->Accept(this);

        indent -= 4;
    }

    void Visit(EnumValue* node) override {
        Print("EnumValue: %s\n", node->name.c_str());
        indent += 2;

        if (node->expression != nullptr)
            node->expression->Accept(this);
        else
            Print("default\n");

        indent -= 2;
    }

    void Visit(EnumConstructor* node) override {
        Print("EnumConstructor: %s\n", node->name.c_str());
        indent += 2;

        for (auto& paramName: node->parameterOrder)
            node->parameters[paramName]->Accept(this);

        indent -= 2;
    }

    void Visit(EnumParameter* node) override {
        if (node->name)
            Print("%s: %s\n", (*node->name).c_str(), node->type->name.c_str());
        else
            Print("(unnamed): %s\n", node->type->name.c_str());
    }

    void Visit(Function* node) override {
        Print("Function: %s\n", std::string(node->name).c_str());
        indent += 2;

        Print("Return type: %s\n", FormatTypeRef(node->returnType).c_str());
        Print("Is External: %s\n", node->isExternal ? "Yes" : "No");

        Print("Parameters: %d\n", node->parameters.size());
        indent += 2;
        for (auto& paramName: node->parameterOrder)
            node->parameters[paramName]->Accept(this);

        indent -= 2;
        Print("Body: %d\n", node->body.size());
        indent += 2;
        for (auto& statement: node->body) {
            statement->Accept(this);
        }

        indent -= 4;
    }

    void Visit(FunctionParameter* node) override {
        Print(
            "%s: %s\n",
            node->name.c_str(),
            FormatTypeRef(node->type).c_str()
        );
    }

    /*
     * Statements
     */

    void Visit(Return* node) override {
        Print("Return:\n");
        indent += 2;

        if (node->expression != nullptr)
            node->expression->Accept(this);
        else
            Print("default\n");

        indent -= 2;
    }

    void Visit(VariableDeclaration* node) override {
        Print("Variable declaration: %s\n", node->name.c_str());
        indent += 2;

        Print("Type: %s\n", FormatTypeRef(node->type).c_str());

        Print("Expression:\n");
        indent += 2;

        if (node->expression != nullptr)
            node->expression->Accept(this);
        else
            Print("default\n");

        indent -= 4;
    }

    void Visit(IfStatement* node) override {
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
        for (auto& statement: node->body)
            statement->Accept(this);
        indent -= 2;

        if (node->elseStatement) {
            Print("Else:\n");
            indent += 2;
            node->elseStatement->Accept(this);
            indent -= 2;
        }
    }

    void Visit(ForLoop* node) override {
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
        for (auto& statement: node->body)
            statement->Accept(this);
        indent -= 2;

        indent -= 2;
    }

    void Visit(Continue* node) override {
        Print("Continue\n");
    }

    void Visit(Break* node) override {
        Print("Break\n");
    }

    /*
     * Expressions
     */

    void Visit(ConstructorCall* node) override {
        Print("Constructor call: %s\n", node->type->ResolveType()->name.c_str());
        indent += 2;

        Print("Arguments: %d\n", node->arguments.size());
        indent += 2;
        for (auto& argument: node->arguments) {
            argument->Accept(this);
        }

        indent -= 4;
    }

    void Visit(FunctionCall* node) override {
        Print("Function call: %s\n", node->name.c_str());
        indent += 2;

        Print("Arguments: %d\n", node->arguments.size());
        indent += 2;
        for (auto& argument: node->arguments)
            argument->Accept(this);
        indent -= 2;

        if (node->next)
            node->next->Accept(this);

        indent -= 2;
    }

    void Visit(BinaryOperation* node) override {
        Print("Binary operation: %s\n", Console::OpToString(node->op).c_str());
        indent += 2;

        Print("Left:\n");
        indent += 2;
        node->lhs->Accept(this);
        indent -= 2;

        Print("Right:\n");
        indent += 2;
        node->rhs->Accept(this);

        indent -= 4;
    }

    void Visit(TypeReference* node) override {
        Print("TypeRef: %s\n", node->name.c_str());
    }

    void Visit(VariableReference* node) override {
        Print("VarRef: %s\n", node->name.c_str());
        indent += 2;
        if (node->next)
            node->next->Accept(this);

        indent -= 2;
    }

    void Visit(ArrayRef* node) override {
        Print("ArrayRef: %d\n", node->index);
        indent += 2;
        if (node->next)
            node->next->Accept(this);

        indent -= 2;
    }

    void Visit(StaticRef* node) override {
        Print("StaticRef: %s\n", node->name.c_str());
        indent += 2;
        if (node->next)
            node->next->Accept(this);

        indent -= 2;
    }

    void Visit(When* node) override {
        Print("When:\n");
        indent += 2;

        Print("Expression:\n");
        indent += 2;
        node->expression->Accept(this);
        indent -= 2;

        Print("Cases: %d\n", node->body.size());
        indent += 2;
        for (auto& item: node->body)
            item->Accept(this);
        indent -= 2;

        indent -= 2;
    }

    void Visit(WhenCondition* node) override {
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
        for (auto& statement: node->body)
            statement->Accept(this);
        indent -= 2;

        indent -= 2;
    }

    /*
     * Literals
     */

    void Visit(Integer* node) override {
        Print("Integer: %d\n", node->value);
    }

    void Visit(Double* node) override {
        Print("Double: %f\n", node->value);
    }

    void Visit(Boolean* node) override {
        Print("Boolean: %d\n", node->value);
    }

    void Visit(String* node) override {
        Print("String: %s\n", node->value.c_str());
    }

    void Visit(Null* node) override {
        Print("Null\n");
    }
};
