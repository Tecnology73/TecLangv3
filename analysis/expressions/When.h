#pragma once

#include <set>
#include "../Analyzer.h"
#include "../../ast/expressions/When.h"
#include "../../context/analysis/WhenAnalysisContext.h"

class WhenAnalyzer : public Analyzer<When> {
public:
    WhenAnalyzer(Visitor* visitor, When* node) : Analyzer(visitor, node) {
    }

    void Analyze() override;

private:
    bool exhaustiveCaseCheck();

    void inferReturnTypes(std::shared_ptr<WhenAnalysisContext> context);

    void reduceReturnTypes(std::set<TypeReference*>& types);
};
