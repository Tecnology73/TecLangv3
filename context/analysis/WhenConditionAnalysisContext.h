#pragma once

#include "../Context.h"
#include "../../ast/expressions/When.h"
#include "WhenAnalysisContext.h"

class WhenConditionAnalysisContext : public Context {
public:
    explicit WhenConditionAnalysisContext(Visitor* visitor, WhenCondition* condition) : Context(visitor),
        condition(condition) {
    }

    TypeReference* getReturnType() override {
        return parent->getReturnType();
    }

    Node* GetNextNode() {
        if (astLoopIndex >= condition->body.size()) return nullptr;
        return condition->body[astLoopIndex++];
    }

    void ReplaceCurrentNode(Node* node) {
        // -1 because we increment when we get the next node.
        auto oldNode = condition->body[--astLoopIndex];
        condition->body[astLoopIndex] = node;
        delete oldNode;
    }

    bool isParentReturn() const {
        return dynamic_cast<WhenAnalysisContext *>(parent)->isParentReturn();
    }

public:
    WhenCondition* const condition;

    unsigned astLoopIndex = 0;
};
