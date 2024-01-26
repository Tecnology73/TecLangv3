#pragma once

#include "../Context.h"
#include "../../ast/statements/IfStatement.h"
#include "../../compiler/Compiler.h"

class IfStatementAnalysisContext : public Context {
public:
    explicit IfStatementAnalysisContext(Visitor* visitor, IfStatement* node) : Context(visitor), ifStatement(node) {
    }

    Node* GetNextNode() {
        if (astLoopIndex >= ifStatement->body.size()) return nullptr;
        return ifStatement->body[astLoopIndex++];
    }

    void ReplaceCurrentNode(Node* node) override {
        // -1 because we increment when we get the next node.
        auto oldNode = ifStatement->body[--astLoopIndex];
        ifStatement->body[astLoopIndex] = node;
        delete oldNode;
    }

    void handleReturn(const Node* node, const TypeReference* type) override {
        hasReturned = true;
        parent->handleReturn(node, type);
    }

    void narrowType(const ChainableNode* node, const TypeReference* type) const override {
        auto fullName = node->GetText();
        // Check to see if the full chain has already been cached.
        auto symbol = Compiler::getScopeManager().GetVar(fullName);
        if (!symbol) {
            // Resolve the full chain.
            symbol = Compiler::getScopeManager().GetVar(node->name);

            auto current = node->next;
            while (symbol && current) {
                symbol = symbol->GetField(current->name);
                current = current->next;
            }
        }

        if (!symbol)
            // Something's serious fucked up if we end up here...
            return;

        Compiler::getScopeManager().Add(fullName, symbol->node, type);
    }

public:
    IfStatement* const ifStatement;
    bool hasReturned = false;

    unsigned astLoopIndex = 0;
};
