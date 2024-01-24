#pragma once

#include "../ast/Visitor.h"

class TypeBase;

class Context {
    friend class ScopeManager;

public:
    explicit Context(Visitor* visitor) : visitor(visitor) {
    }

    virtual ~Context() = default;

    virtual TypeVariant* getReturnType() {
        return nullptr;
    }

    virtual void ReplaceCurrentNode(Node* node) {
        if (parent) {
            parent->ReplaceCurrentNode(node);
            return;
        }

        visitor->SetAstNode(visitor->GetAstLoopIndex(), node);
        visitor->DecAstLoop();
    }

    virtual void handleReturn(const Node* node) {
        if (parent) {
            parent->handleReturn(node);
            return;
        }

        throw std::runtime_error("Return not implemented in this context.");
    }

    virtual void handleReturn(const Node* node, llvm::Value* value) {
        if (parent) {
            parent->handleReturn(node, value);
            return;
        }

        throw std::runtime_error("Return not implemented in this context.");
    }

    virtual void handleReturn(const Node* node, const TypeVariant* value) {
        if (parent) {
            parent->handleReturn(node, value);
            return;
        }

        throw std::runtime_error("Return not implemented in this context.");
    }

    virtual void handleContinue(const Node* node) {
        throw std::runtime_error("Continue not implemented in this context.");
    }

    virtual void handleBreak(const Node* node) {
        throw std::runtime_error("Break not implemented in this context.");
    }

    virtual void narrowType(const ChainableNode* node, const TypeVariant* variant) const {
        throw std::runtime_error("Cannot narrow type in this context.");
    }

protected:
    Visitor* const visitor;
    Context* parent = nullptr;
};
