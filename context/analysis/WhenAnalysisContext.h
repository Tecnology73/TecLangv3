#pragma once

#include "ReturnAnalysisContext.h"
#include "../Context.h"
#include "../../ast/expressions/When.h"

class WhenAnalysisContext : public Context {
public:
    explicit WhenAnalysisContext(Visitor* visitor, When* when) : Context(visitor), when(when) {
    }

    TypeReference* getReturnType() override {
        return when->returnType;
    }

    bool isParentReturn() const {
        return dynamic_cast<ReturnAnalysisContext *>(parent) != nullptr;
    }

public:
    When* const when;
};
