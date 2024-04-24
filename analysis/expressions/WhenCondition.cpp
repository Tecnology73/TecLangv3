#include "WhenCondition.h"
#include "../../context/analysis/WhenConditionAnalysisContext.h"
#include "../../scope/Scope.h"

void WhenConditionAnalyzer::Analyze() {
    // Create a context
    auto [scope, context] = Scope::Enter<WhenConditionAnalysisContext>(visitor, node);

    // Not present for the `else` case.
    if (node->condition) {
        node->condition->Accept(visitor);
        if (VisitorResult result; !visitor->TryGetResult(result)) return;
    }

    // Visit everything in the body
    while (const auto& item = context->GetNextNode()) {
        item->Accept(visitor);

        VisitorResult result;
        if (!visitor->TryGetResult(result)) return;

        if (!node->implicitReturn) continue;

        context->handleReturn(item, result.type);
    }

    // Cleanup
    scope->Leave();

    visitor->AddSuccess();
}
