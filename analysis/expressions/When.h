#pragma once

#include "../Analyzer.h"
#include "../../ast/expressions/When.h"

class WhenAnalyzer : public Analyzer<When> {
public:
    WhenAnalyzer(Visitor *visitor, When *node) : Analyzer(visitor, node) {}

    void Analyze() override;

private:
    bool exhaustiveCaseCheck();

    bool inferReturnType();
};
