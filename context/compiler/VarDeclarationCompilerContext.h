#pragma once

#include "../Context.h"
#include "../../ast/statements/VarDeclaration.h"

class VarDeclarationCompilerContext : public Context {
public:
    explicit VarDeclarationCompilerContext(Visitor* visitor, VariableDeclaration* var) : Context(visitor), var(var) {
    }

    TypeVariant* getReturnType() override {
        return var->type->ResolveType();
    }

public:
    VariableDeclaration* const var;
};
