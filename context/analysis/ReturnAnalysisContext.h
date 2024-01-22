#pragma once

#include "../Context.h"
#include "../../ast/Statements.h"
#include "FunctionAnalysisContext.h"

class ReturnAnalysisContext : public Context {
public:
    explicit ReturnAnalysisContext(Visitor* visitor, Return* aReturn) : Context(visitor), aReturn(aReturn) {
    }

    TypeVariant* getReturnType() override {
        if (auto functionContext = dynamic_cast<FunctionAnalysisContext *>(parent))
            return functionContext->getReturnType();

        return nullptr;
    }

    void handleReturn(const Node* node, llvm::Value* value) override {
        // if (auto functionContext = dynamic_cast<FunctionAnalysisContext *>(parent))
        // functionContext->addReturnType(node, value);
    }

public:
    Return* const aReturn;
};
