#include "Return.h"
#include "../../compiler/Compiler.h"

void ReturnAnalyzer::Analyze() {
    if (!node->expression) {
        Compiler::getScopeManager().getContext()->handleReturn(
            node,
            Compiler::getScopeManager().getType("void")->createVariant()
        );
        return;
    }

    node->expression->Accept(visitor);

    VisitorResult result;
    if (!visitor->TryGetResult(result)) return;

    Compiler::getScopeManager().getContext()->handleReturn(node, result.type);
    visitor->AddSuccess(result.type);
}
