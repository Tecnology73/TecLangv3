#pragma once

#include "../Context.h"
#include "FunctionAnalysisContext.h"

class ReturnAnalysisContext : public Context {
public:
    explicit ReturnAnalysisContext(Visitor* visitor, Return* aReturn) : Context(visitor), aReturn(aReturn) {
    }

    TypeReference* getReturnType() override {
        if (auto functionContext = dynamic_cast<FunctionAnalysisContext *>(parent))
            return functionContext->getReturnType();

        return nullptr;
    }

public:
    Return* const aReturn;
};
