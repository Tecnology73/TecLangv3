#pragma once

#include "../Context.h"
#include "../../ast/statements/VarDeclaration.h"

class VarDeclarationCompilerContext : public Context {
public:
    explicit VarDeclarationCompilerContext(Visitor* visitor, VariableDeclaration* var) : Context(visitor), var(var) {
    }

    TypeReference* getReturnType() override {
        return var->type;
    }

public:
    VariableDeclaration* const var;
};
