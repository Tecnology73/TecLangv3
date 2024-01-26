#pragma once

#include "../Context.h"
#include "../../ast/Statements.h"
#include "../../ast/toplevel/Function.h"

class FunctionAnalysisContext : public Context {
public:
    explicit FunctionAnalysisContext(Visitor* visitor, Function* function) : Context(visitor), function(function) {
    }

    TypeReference* getReturnType() override {
        return function->returnType;
    }

    void handleReturn(const Node* node) override {
    }

    void handleReturn(const Node* node, llvm::Value* value) override {
    }

    void handleReturn(const Node* node, const TypeReference* type) override {
    }

    Node* GetNextNode() {
        if (astLoopIndex >= function->body.size()) return nullptr;
        return function->body[astLoopIndex++];
    }

    void ReplaceCurrentNode(Node* node) override {
        // -1 because we increment when we get the next node.
        auto oldNode = function->body[--astLoopIndex];
        function->body[astLoopIndex] = node;
        delete oldNode;
    }

public:
    Function* const function;

    unsigned astLoopIndex = 0;
};
