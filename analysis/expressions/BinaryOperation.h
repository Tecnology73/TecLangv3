#pragma once

#include "../Analyzer.h"
#include "../../ast/expressions/BinaryOperation.h"

class BinaryOperationAnalyzer : public Analyzer<BinaryOperation> {
public:
    using Analyzer::Analyzer;

    BinaryOperationAnalyzer(Visitor* visitor, BinaryOperation* node) : Analyzer(visitor, node) {
    }

    void Analyze() override;

private:
    bool tryPromoteToVarDecl();

    void analyzeAssign();

    void analyzeIs();
};
