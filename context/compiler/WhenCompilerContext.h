#pragma once

#include <llvm/IR/BasicBlock.h>
#include "../Context.h"
#include "../../ast/expressions/When.h"

class WhenCompilerContext : public Context {
public:
    explicit WhenCompilerContext(Visitor* visitor, When* when);

    TypeReference* getReturnType() override;

    void handleReturn(Node* node, llvm::Value* value) override;

    bool isParentReturn() const;

public:
    When* const when;
    llvm::BasicBlock* exitBlock;
};
