#pragma once

#include <llvm/IR/BasicBlock.h>
#include "../Context.h"
#include "../../ast/statements/IfStatement.h"

class IfCompilerContext : public Context {
public:
    explicit IfCompilerContext(Visitor* visitor, IfStatement* node) : Context(visitor), node(node) {
    }

    void handleReturn(Node* node, llvm::Value* value) override {
        parent->handleReturn(node, value);
    }

    bool isParentIf() const {
        return parent && dynamic_cast<IfCompilerContext *>(parent);
    }

    llvm::BasicBlock* getExitBlock() const {
        if (auto ifParent = dynamic_cast<IfCompilerContext *>(parent))
            return ifParent->exitBlock;

        return exitBlock;
    }

public:
    IfStatement* const node;
    llvm::BasicBlock* exitBlock = nullptr;
};
