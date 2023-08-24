#pragma once

#include "CodegenContext.h"
#include "../../ast/Statements.h"

class ReturnContext : public CodegenContext {
public:
    explicit ReturnContext(Visitor *visitor, Return *aReturn) : CodegenContext(visitor), aReturn(aReturn) {}

    TypeDefinition *getReturnType() override;

    void handleReturn(const Node *node, llvm::Value *value) override;

public:
    Return *const aReturn;
};