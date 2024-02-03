#pragma once

#include "../Context.h"
#include "../../ast/toplevel/Function.h"

class FunctionContext : public Context {
public:
    explicit FunctionContext(Visitor* visitor, Function* function) : Context(visitor), function(function) {
    }

    void handleReturn(Node* node, const TypeReference* type) override {
        function->analysisInfo->AddReturnType(node, type);
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
