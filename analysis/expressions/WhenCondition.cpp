#include "WhenCondition.h"
#include "../../compiler/Compiler.h"

void WhenConditionAnalyzer::Analyze() {
    // Create a context
    auto context = Compiler::getScopeManager().enter("whenCondition", new WhenConditionAnalysisContext(visitor, node));

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
    Compiler::getScopeManager().popContext();
    Compiler::getScopeManager().leave("whenCondition");

    visitor->AddSuccess();
}