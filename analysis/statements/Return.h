#pragma once

#include "../Analyzer.h"
#include "../../ast/statements/Return.h"

class ReturnAnalyzer : public Analyzer<Return> {
public:
    ReturnAnalyzer(Visitor* visitor, Return* node) : Analyzer(visitor, node) {
    }

    void Analyze() override;
};
