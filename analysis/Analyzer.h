#pragma once

#include "../ast/Visitor.h"

template<typename T>
class Analyzer {
public:
    Analyzer(Visitor *visitor, T *node) : visitor(visitor), node(node) {}

    virtual ~Analyzer() = default;

    virtual void Analyze() = 0;

protected:
    Visitor *visitor;
    T *node;
};
