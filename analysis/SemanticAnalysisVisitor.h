#pragma once

#include "../ast/Visitor.h"
#include "../compiler/Compiler.h"
#include "../symbolTable/SymbolTable.h"
//
#include "topLevel/Enum.h"
#include "topLevel/Function.h"
//
#include "expressions/When.h"
#include "expressions/WhenCondition.h"
#include "expressions/BinaryOperation.h"
//
#include "statements/IfStatement.h"
#include "statements/Return.h"
//
#include "../ast/expressions/StaticRef.h"
#include "../ast/expressions/ConstructorCall.h"
#include "../ast/expressions/VarReference.h"
#include "../ast/Literals.h"

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

        for (const auto& functions: node->functions | std::views::values) {
            for (const auto& function: functions) {
                function->Accept(this);

                if (VisitorResult result; !TryGetResult(result)) return;
            }
        }

        AddSuccess();
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

    void Visit(Function* node) override {
        FunctionAnalyzer(this, node).Analyze();
    }

    void Visit(class FunctionParameter* node) override {
        if (!node->type->ResolveType()) {
            ReportError(ErrorCode::TYPE_UNKNOWN, {node->type->name}, node->type);
            return;
        }

        Compiler::getScopeManager().Add(node);
        AddSuccess();
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
                node->type = result.type->Clone();
        } else if (!node->type->flags.Has(TypeFlag::OPTIONAL)) {
            auto resolvedType = node->type->ResolveType();
            if (!resolvedType) {
                ReportError(ErrorCode::TYPE_UNKNOWN, {node->type->name}, node->type);
                return;
            }

            if (!resolvedType->isValueType)
                // This is not a syntax error so we can continue.
                ReportError(
                    ErrorCode::VALUE_CANNOT_BE_NULL,
                    {
                        node->name,
                        node->type->name,
                    },
                    node
                );
        }

        Compiler::getScopeManager().Add(node);
        AddSuccess(node->type);
    }

    void Visit(class IfStatement* node) override {
        IfStatementAnalyzer(this, node).Analyze();
    }

    void Visit(class ForLoop* node) override {
        node->value->Accept(this);

        if (node->step) {
            node->step->Accept(this);

            if (VisitorResult result; !TryGetResult(result)) return;
        }

        if (node->identifier) {
            node->identifier->Accept(this);

            if (VisitorResult result; !TryGetResult(result)) return;
        }

        for (const auto& item: node->body) {
            item->Accept(this);

            if (VisitorResult result; !TryGetResult(result)) return;
        }

        AddSuccess();
    }

    void Visit(class Continue* node) override {
    }

    void Visit(class Break* node) override {
    }

    /*
     * Expressions
     */

    void Visit(class ConstructorCall* node) override {
        auto type = node->type;
        if (!type->ResolveType()) {
            ReportError(ErrorCode::TYPE_UNKNOWN, {node->type->name}, node->type);
            return;
        }

        // Visit(dynamic_cast<TypeDefinition *>(type->type));
        // if (VisitorResult result; !TryGetResult(result)) return;

        AddSuccess(type);
    }

    void Visit(class FunctionCall* node) override {
        if (!node->function->returnType) {
            Visit(node->function);

            if (VisitorResult result; !TryGetResult(result)) return;
        }

        if (!node->function->returnType) {
            AddFailure();
            return;
        }

        AddSuccess(node->function->returnType);
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
                // FIXME: Temporary
                if (auto func = symbol->narrowedType->ResolveType()->GetFunction(current->name)) {
                    AddSuccess(func->returnType);
                    return;
                }

                ReportError(ErrorCode::TYPE_UNKNOWN_FIELD, {current->name, symbol->narrowedType->name}, current);
                return;
            }

            if (field->narrowedType->flags.Has(TypeFlag::OPTIONAL)) {
                ErrorManager::QueueHint("Check if the value is null or use optional chaining.", {});
                ReportError(ErrorCode::VALUE_POSSIBLY_NULL, {}, current);
                return;
            }

            // TODO: Support other types (like arrays) that proxy to their internal data.
            // Any string that is a part of a chain should load its internal data field.
            if (field->narrowedType->name == "string" && current->next)
                current->loadInternalData = true;

            current = current->next;
            symbol = field;
        }

        AddSuccess(symbol->narrowedType);
    }

    void Visit(class StaticRef* node) override {
        auto anEnum = SymbolTable::GetInstance()->Get<Enum>(node->name);
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
        WhenConditionAnalyzer(this, node).Analyze();
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
