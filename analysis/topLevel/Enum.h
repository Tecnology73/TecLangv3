#pragma once

#include "../Analyzer.h"
#include "../../ast/topLevel/Enum.h"

class EnumAnalyzer : public Analyzer<Enum> {
public:
    EnumAnalyzer(Visitor *visitor, Enum *node) : Analyzer(visitor, node) {}

    void Analyze() override;

    static Enum *lastEnum;
};
