#pragma once

#include "../Analyzer.h"
#include "../../ast/expressions/StaticRef.h"
#include "../../ast/expressions/VarReference.h"
#include "../../ast/expressions/FunctionCall.h"
#include "../../context/analysis/StaticRefAnalysisContext.h"

class StaticRefAnalyzer : public Analyzer<StaticRef> {
public:
    using Analyzer::Analyzer;

    void Analyze() override;

private:
    void AnalyzeField(std::shared_ptr<StaticRefAnalysisContext> context, VariableReference* ref);

    void AnalyzeConstructor(std::shared_ptr<StaticRefAnalysisContext> context, FunctionCall* fnCall);
};
