#pragma once

#include "../Analyzer.h"
#include "../../ast/expressions/When.h"

class WhenConditionAnalyzer : public Analyzer<WhenCondition> {
public:
    WhenConditionAnalyzer(Visitor* visitor, WhenCondition* node) : Analyzer(visitor, node) {
    }

    void Analyze() override;
};
