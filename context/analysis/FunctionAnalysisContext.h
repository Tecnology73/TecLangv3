#pragma once

#include "../Context.h"
#include "../../ast/Statements.h"
#include "../../ast/toplevel/Function.h"

class FunctionAnalysisContext : public Context {
public:
    explicit FunctionAnalysisContext(Visitor* visitor, Function* function) : Context(visitor), function(function) {
    }

    TypeVariant* getReturnType() override {
        return function->returnType->ResolveType();
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

    void handleReturn(const Node* node, const TypeVariant* type) override {
        if (!returnStatements.contains(type))
            returnStatements.emplace(type, std::vector<const Return *>());

        returnStatements[type].emplace_back(dynamic_cast<const Return *>(node));
    }

public:
    Function* const function;
    // Keep track of return statements, so we can infer the return type if required.
    std::map<const TypeVariant *, std::vector<const Return *>> returnStatements;

    unsigned astLoopIndex = 0;
};
