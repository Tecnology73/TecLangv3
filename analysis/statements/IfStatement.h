#pragma once

#include "../Analyzer.h"
#include "../../ast/statements/IfStatement.h"
#include "../../context/analysis/IfStatementAnalysisContext.h"

class IfStatementAnalyzer : public Analyzer<IfStatement> {
public:
    IfStatementAnalyzer(Visitor* visitor, IfStatement* node) : Analyzer(visitor, node) {
    }

    void Analyze() override;

private:
    bool analyzeCondition();

    bool analyzeBody(IfStatementAnalysisContext* context);

    bool analyzeElse();

    void propagateNarrowedTypes();
};
