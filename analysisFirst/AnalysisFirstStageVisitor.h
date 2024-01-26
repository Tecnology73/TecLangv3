#pragma once

#include "../ast/Visitor.h"
#include "../symbolTable/SymbolTable.h"
//
#include "../ast/topLevel/Function.h"
//
#include "../ast/statements/Return.h"
//
#include "../ast/expressions/FunctionCall.h"
//
#include "../ast/Literals.h"

class FunctionContext {
public:
    explicit FunctionContext(Function* function) : function(function) {
    }

    void AddReturnType(Node* node, const TypeReference* type = nullptr) const {
        function->analysisInfo->AddReturnType(node, type);
    }

    Function* function;
};

class AnalysisFirstStageVisitor : public Visitor {
public:
    FunctionContext* context = nullptr;

    explicit AnalysisFirstStageVisitor(Parser* p) : Visitor("AnalysisFirstStageVisitor", p) {
    }

    /*
     * Top level
     */

    void Visit(Function* node) override {
        node->analysisInfo = new AnalysisInfo(node);
        context = new FunctionContext(node);

        for (const auto& item: node->body) {
            item->Accept(this);

            GetResult();
        }

        // If there are no unknow return types, we can try to resolve the return type now.
        if (!node->returnType && node->analysisInfo->unknownReturnTypes.empty()) {
            if (node->analysisInfo->possibleReturnTypes.empty())
                node->returnType = SymbolTable::GetInstance()->GetReference("void");
            else if (node->analysisInfo->possibleReturnTypes.size() == 1)
                node->returnType = SymbolTable::GetInstance()->GetReference(
                    std::string(node->analysisInfo->possibleReturnTypes.begin()->first)
                );
            else
                ReportError(ErrorCode::FUNCTION_MULTIPLE_RETURN_TYPES, {node->name}, node);
        }

        delete context;
        AddSuccess();
    }

    /*
     * Statements
     */

    void Visit(Return* node) override {
        if (!node->expression) {
            context->AddReturnType(node, SymbolTable::GetInstance()->GetReference("void"));
            AddSuccess();
            return;
        }

        node->expression->Accept(this);

        auto result = GetResult();
        if (!result.success)
            context->AddReturnType(node);
        else
            context->AddReturnType(node, result.type);

        AddSuccess();
    }

    /*
     * Expressions
     */

    void Visit(FunctionCall* node) override {
        std::vector<TypeReference *> args;
        args.reserve(node->arguments.size());
        for (const auto& arg: node->arguments) {
            arg->Accept(this);

            VisitorResult result;
            if (!TryGetResult(result)) return;

            args.push_back(result.type);
        }

        node->function = SymbolTable::GetInstance()->LookupFunction(node->name, args);
        if (!node->function || !node->function->returnType) {
            AddFailure();
            return;
        }

        AddSuccess(node->function->returnType);
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

    /*
     * Literals
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

    void Visit(class FunctionParameter* node) override {
    }

    void Visit(class VariableDeclaration* node) override {
    }

    void Visit(class IfStatement* node) override {
    }

    void Visit(class ForLoop* node) override {
    }

    void Visit(class Continue* node) override {
    }

    void Visit(class Break* node) override {
    }

    void Visit(class ConstructorCall* node) override {
    }

    void Visit(class BinaryOperation* node) override {
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

    void Visit(class String* node) override {
    }

    void Visit(class Null* node) override {
    }
};
