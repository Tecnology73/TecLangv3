#pragma once

#include <llvm/IR/Value.h>
#include "../parser/Parser.h"
#include "../compiler/ErrorManager.h"
#include "topLevel/TypeBase.h"

struct VisitorResult {
    bool success = false;
    llvm::Value* value = nullptr;
    TypeReference* type = nullptr;
    ErrorCode errorCode = ErrorCode::UNKNOWN_ERROR;
};

class Visitor {
public:
    Visitor(std::string n, Parser* p) : name(std::move(n)), parser(p) {
    }

    virtual ~Visitor() = default;

    void ReportError(
        const ErrorCode errorCode,
        const std::vector<std::string>& args,
        const Node* node
    ) {
        ErrorManager::Report(errorCode, args, parser, node);
        AddFailure(errorCode);
    }

    void AddSuccess() {
        results.emplace(true, nullptr, nullptr);
    }

    void AddSuccess(llvm::Value* value) {
        results.emplace(true, value, nullptr);
    }

    void AddSuccess(TypeReference* type) {
        results.emplace(true, nullptr, type);
    }

    void AddSuccess(llvm::Value* value, TypeReference* type) {
        results.emplace(true, value, type);
    }

    void AddFailure() {
        results.emplace(false);
    }

    void AddFailure(ErrorCode code) {
        results.emplace(false, nullptr, nullptr, code);
    }

    void AddFailure(VisitorResult result) {
        results.emplace(result);
    }

    bool HasResults() const {
        return !results.empty();
    }

    VisitorResult GetResult() {
        if (results.empty())
            return {false};

        auto result = results.top();
        results.pop();

        return result;
    }

    /// <summary>
    /// This should only be used after the visitor has visited all nodes
    /// and we're trying to output any errors.
    /// </summary>
    std::stack<VisitorResult>& GetResults() {
        return results;
    }

    template<unsigned int N>
    std::array<VisitorResult, N> GetResults() {
        std::array<VisitorResult, N> resultsArray;

        for (unsigned int i = 0; i < N; i++)
            resultsArray[N - i - 1] = GetResult();

        return resultsArray;
    }

    /// <summary>
    /// Try to get the latest result.
    /// If there are no results, return false.
    /// If the last result was an error, return false.
    /// Otherwise, return true and pop the result.
    /// </summary>
    bool TryGetResult(VisitorResult& result) {
        if (results.empty())
            return false;

        result = results.top();
        if (result.success)
            results.pop();

        return result.success;
    }

    unsigned GetAstLoopIndex() const {
        return parser->GetAstLoopIndex();
    }

    void DecAstLoop() const {
        return parser->DecAstLoop();
    }

    void SetAstNode(const unsigned index, Node* node) const {
        return parser->SetAstNode(index, node);
    }

    /*
     * Top level
     */

    virtual void Visit(class TypeField* node) = 0;

    virtual void Visit(class TypeDefinition* node) = 0;

    virtual void Visit(class Enum* node) = 0;

    virtual void Visit(class EnumValue* node) = 0;

    virtual void Visit(class EnumConstructor* node) = 0;

    virtual void Visit(class EnumParameter* node) = 0;

    virtual void Visit(class Function* node) = 0;

    virtual void Visit(class FunctionParameter* node) = 0;

    /*
     * Statements
     */

    virtual void Visit(class Return* node) = 0;

    virtual void Visit(class VariableDeclaration* node) = 0;

    virtual void Visit(class IfStatement* node) = 0;

    virtual void Visit(class ForLoop* node) = 0;

    virtual void Visit(class Continue* node) = 0;

    virtual void Visit(class Break* node) = 0;

    /*
     * Expressions
     */

    virtual void Visit(class ConstructorCall* node) = 0;

    virtual void Visit(class FunctionCall* node) = 0;

    virtual void Visit(class BinaryOperation* node) = 0;

    virtual void Visit(class TypeReference* node) = 0;

    virtual void Visit(class VariableReference* node) = 0;

    virtual void Visit(class StaticRef* node) = 0;

    virtual void Visit(class When* node) = 0;

    virtual void Visit(class WhenCondition* node) = 0;

    /*
     * Literals
     */

    virtual void Visit(class Integer* node) = 0;

    virtual void Visit(class Double* node) = 0;

    virtual void Visit(class Boolean* node) = 0;

    virtual void Visit(class String* node) = 0;

    virtual void Visit(class Null* node) = 0;

public:
    std::string const name;

private:
    Parser* parser;
    std::stack<VisitorResult> results;
};
