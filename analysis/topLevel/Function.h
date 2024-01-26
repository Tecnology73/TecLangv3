#pragma once

#include <set>
#include "../Analyzer.h"
#include "../../ast/topLevel/Function.h"
#include "../../context/analysis/FunctionAnalysisContext.h"

class FunctionAnalyzer : public Analyzer<Function> {
public:
    FunctionAnalyzer(Visitor* visitor, Function* node) : Analyzer(visitor, node) {
    }

    void Analyze() override;

private:
    bool inferReturnTypes(const FunctionAnalysisContext* context);

    void reduceReturnTypes(std::set<TypeReference *>& types);
};
