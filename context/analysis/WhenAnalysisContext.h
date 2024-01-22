#pragma once

#include "ReturnAnalysisContext.h"
#include "../Context.h"
#include "../../ast/expressions/When.h"

class WhenAnalysisContext : public Context {
public:
    explicit WhenAnalysisContext(Visitor* visitor, When* when) : Context(visitor), when(when) {
    }

    TypeVariant* getReturnType() override {
        return when->returnType;
    }

    void handleReturn(const Node* node, const TypeVariant* type) override {
        if (!returnStatements.contains(type))
            returnStatements.emplace(type, std::vector<const Return *>());

        returnStatements[type].emplace_back(dynamic_cast<const Return *>(node));
    }

    bool isParentReturn() const {
        return dynamic_cast<ReturnAnalysisContext *>(parent) != nullptr;
    }

public:
    When* const when;
    std::map<const TypeVariant *, std::vector<const Return *>> returnStatements;
};
