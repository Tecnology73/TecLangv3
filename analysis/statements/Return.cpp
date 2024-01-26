#include "Return.h"
#include "../../compiler/Compiler.h"
#include "../../symbolTable/SymbolTable.h"

void ReturnAnalyzer::Analyze() {
    if (!node->expression) {
        Compiler::getScopeManager().getContext()->handleReturn(
            node,
            SymbolTable::GetInstance()->GetReference("void")
        );
        return;
    }

    node->expression->Accept(visitor);

    VisitorResult result;
    if (!visitor->TryGetResult(result)) return;

    Compiler::getScopeManager().getContext()->handleReturn(node, result.type);
    visitor->AddSuccess(result.type);
}
