#pragma once

#include "../ast/Visitor.h"
#include "../compiler/Compiler.h"
//
#include "topLevel/Enum.h"
#include "topLevel/Function.h"
//
#include "expressions/When.h"
#include "expressions/BinaryOperation.h"
//
#include "statements/IfStatement.h"
#include "statements/Return.h"

class SemanticAnalysisVisitor : public Visitor {
public:
    explicit SemanticAnalysisVisitor(Parser* p) : Visitor("SemanticAnalysisVisitor", p) {
    }

    /*
     * Top level
     */

    void Visit(class TypeField* node) override {
    }

    void Visit(class TypeDefinition* node) override {
        if (!node->isDeclared) {
            ReportError(ErrorCode::TYPE_UNKNOWN, {node->name}, node);
            return;
        }
    }

    void Visit(class Enum* node) override {
        EnumAnalyzer(this, node).Analyze();
    }

    void Visit(class EnumValue* node) override {
    }

    void Visit(class EnumConstructor* node) override {
    }

    void Visit(class EnumParameter* node) override {
    }

    void Visit(class Function* node) override {
        FunctionAnalyzer(this, node).Analyze();
    }

    void Visit(class FunctionParameter* node) override {
    }

    /*
     * Statements
     */

    void Visit(class Return* node) override {
        ReturnAnalyzer(this, node).Analyze();
    }

    void Visit(class VariableDeclaration* node) override {
        if (Compiler::getScopeManager().HasVar(node->name)) {
            ReportError(ErrorCode::VARIABLE_ALREADY_DECLARED, {node->name}, node);
            return;
        }

        if (node->expression) {
            node->expression->Accept(this);

            VisitorResult result;
            if (!TryGetResult(result)) return;

            if (!node->type)
                node->type = result.type->CreateReference();
        } else if (!node->type->flags.Has(TypeFlag::OPTIONAL) && !node->type->ResolveType()->type->isValueType) {
            ReportError(
                ErrorCode::VALUE_CANNOT_BE_NULL,
                {
                    node->name,
                    node->type->name,
                },
                node
            );

            // This is not a syntax error so we can continue.
        }

        Compiler::getScopeManager().Add(node);
        AddSuccess(node->type->ResolveType());
    }

    void Visit(class IfStatement* node) override {
        IfStatementAnalyzer(this, node).Analyze();
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
        AddSuccess(node->type->ResolveType());
    }

    void Visit(class FunctionCall* node) override {
        for (const auto& argument: node->arguments) {
            argument->Accept(this);

            if (VisitorResult result; !TryGetResult(result)) return;
        }

        AddSuccess(node->getFinalType());
    }

    void Visit(class BinaryOperation* node) override {
        BinaryOperationAnalyzer(this, node).Analyze();
    }

    void Visit(TypeReference* node) override {
    }

    void Visit(class VariableReference* node) override {
        auto symbol = Compiler::getScopeManager().GetVar(node->name);
        if (!symbol) {
            ReportError(ErrorCode::VARIABLE_UNDECLARED, {node->name}, node);
            return;
        }

        // We only want to check for the optional flag when we're accessing a child field.
        // This allows for `foo = null` to pass through fine
        // but will error for `foo.bar = null` (when foo: Foo?).
        if (node->next && symbol->narrowedType->flags.Has(TypeFlag::OPTIONAL)) {
            ErrorManager::QueueHint("Check if the value is null or use optional chaining.", {});
            ReportError(ErrorCode::VALUE_POSSIBLY_NULL, {}, node);
            return;
        }

        auto current = node->next;
        while (current) {
            auto field = symbol->GetField(current->name);
            if (!field) {
                ReportError(ErrorCode::TYPE_UNKNOWN_FIELD, {current->name, symbol->narrowedType->type->name}, current);
                return;
            }

            if (field->narrowedType->flags.Has(TypeFlag::OPTIONAL)) {
                ErrorManager::QueueHint("Check if the value is null or use optional chaining.", {});
                ReportError(ErrorCode::VALUE_POSSIBLY_NULL, {}, current);
                return;
            }

            current = current->next;
            symbol = field;
        }

        AddSuccess(symbol->narrowedType);
    }

    void Visit(class StaticRef* node) override {
        auto anEnum = Compiler::getScopeManager().getEnum(node->name);
        if (!anEnum) {
            ReportError(ErrorCode::STATIC_REF_UNKNOWN, {node->name}, node);
            return;
        }

        auto key = anEnum->GetField(node->next->name);
        if (!key) {
            ReportError(ErrorCode::ENUM_UNKNOWN_VALUE, {node->next->name, anEnum->name}, node->next);
            return;
        }

        key->expression->Accept(this);
    }

    void Visit(class When* node) override {
        WhenAnalyzer(this, node).Analyze();
    }

    void Visit(class WhenCondition* node) override {
        // Not present for the `else` case.
        if (node->condition) {
            node->condition->Accept(this);
            if (VisitorResult result; !TryGetResult(result)) return;
        }

        for (const auto& item: node->body) {
            item->Accept(this);

            VisitorResult result;
            if (!TryGetResult(result)) return;

            if (!node->implicitReturn) continue;

            Compiler::getScopeManager().getContext()->handleReturn(item, result.type);
        }

        AddSuccess();
    }

    /*
     * Literals
     */
    void Visit(class Integer* node) override {
        AddSuccess(node->getType());
    }

    void Visit(class Double* node) override {
        AddSuccess(node->getType());
    }

    void Visit(class Boolean* node) override {
        AddSuccess(node->getType());
    }

    void Visit(class String* node) override {
        AddSuccess(node->getType());
    }

    void Visit(class Null* node) override {
        AddSuccess(node->getType());
    }
};
