#pragma once

#include "CodegenContext.h"

class WhenContext : public CodegenContext {
public:
    explicit WhenContext(Visitor *visitor, When *when);

    TypeDefinition *getReturnType() override;

    void handleReturn(const Node *node, llvm::Value *value) override;

    bool isParentReturn() const;

public:
    When *const when;
    llvm::BasicBlock *exitBlock;
};