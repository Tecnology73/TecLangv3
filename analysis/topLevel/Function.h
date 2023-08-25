#pragma once

#include "../Analyzer.h"
#include "../../ast/topLevel/Function.h"

class FunctionAnalyzer : public Analyzer<Function> {
public:
    FunctionAnalyzer(Visitor *visitor, Function *node) : Analyzer(visitor, node) {}

    void Analyze() override;
};
