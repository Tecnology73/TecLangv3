#pragma once

#include <llvm/IR/Value.h>
#include "../../ast/Visitor.h"

class CodegenContext {
    friend class ScopeManager;

public:
    explicit CodegenContext(Visitor *visitor) : visitor(visitor) {}

    virtual ~CodegenContext() = default;

    virtual TypeDefinition *getReturnType() {
        return nullptr;
    }

    virtual void handleReturn(const Node *node, llvm::Value *value) {
        throw std::runtime_error("Return not implemented in this context.");
    }

    virtual void handleContinue(const Node *node) {
        throw std::runtime_error("Continue not implemented in this context.");
    }

    virtual void handleBreak(const Node *node) {
        throw std::runtime_error("Break not implemented in this context.");
    }

protected:
    Visitor *const visitor;
    CodegenContext *parent = nullptr;
};