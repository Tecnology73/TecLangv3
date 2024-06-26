#pragma once

#include "../ast/Visitor.h"

class TypeBase;

class Context {
public:
    explicit Context(Visitor* visitor) : visitor(visitor) {
    }

    virtual ~Context() = default;

    virtual TypeReference* getReturnType() {
        if (parent)
            return parent->getReturnType();

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

    virtual void handleReturn(Node* node) {
        if (parent) {
            parent->handleReturn(node);
            return;
        }

        throw std::runtime_error("Return not implemented in this context.");
    }

    virtual void handleReturn(Node* node, llvm::Value* value) {
        if (parent) {
            parent->handleReturn(node, value);
            return;
        }

        throw std::runtime_error("Return not implemented in this context.");
    }

    virtual void handleReturn(Node* node, const TypeReference* type) {
        if (parent) {
            parent->handleReturn(node, type);
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

    virtual void narrowType(const ChainableNode* node, const TypeReference* type) const {
        throw std::runtime_error("Cannot narrow type in this context.");
    }

protected:
    Visitor* const visitor;
    Context* parent = nullptr;
};
