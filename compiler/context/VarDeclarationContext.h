#pragma once

#include "CodegenContext.h"
#include "../../ast/Statements.h"

class VarDeclarationContext : public CodegenContext {
public:
    explicit VarDeclarationContext(Visitor *visitor, VariableDeclaration *var) : CodegenContext(visitor), var(var) {}

    TypeDefinition *getReturnType() override {
        // return var->type;
        return nullptr;
    }

public:
    VariableDeclaration *const var;
};