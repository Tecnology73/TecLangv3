#pragma once

#include "../Context.h"
#include "../../ast/expressions/StaticRef.h"
#include "../../ast/topLevel/Enum.h"

class StaticRefAnalysisContext : public Context {
public:
    explicit StaticRefAnalysisContext(Visitor* visitor, StaticRef* node) : Context(visitor), node(node) {
    }
    
public:
    StaticRef* const node;
    Enum* anEnum;
};
