#include "Return.h"
#include "../../compiler/Compiler.h"
#include "../../symbolTable/SymbolTable.h"
#include "../../scope/Scope.h"

void ReturnAnalyzer::Analyze() {
    if (!node->expression) {
        Scope::GetContext()->handleReturn(
            node,
            SymbolTable::GetInstance()->GetReference("void")
        );
        return;
    }

    node->expression->Accept(visitor);

    VisitorResult result;
    if (!visitor->TryGetResult(result)) return;

    Scope::GetContext()->handleReturn(node, result.type);
    visitor->AddSuccess(result.type);
}
