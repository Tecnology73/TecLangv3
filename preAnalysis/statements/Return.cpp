#include "Return.h"
#include "../../compiler/Compiler.h"
#include "../../context/preAnalysis/FunctionContext.h"
#include "../../symbolTable/SymbolTable.h"

void analyseReturn(Visitor* visitor, Return* node) {
    auto context = Compiler::getScopeManager().getContext<FunctionContext>();
    if (!node->expression) {
        context->handleReturn(node, SymbolTable::GetInstance()->GetReference("void"));
        visitor->AddSuccess();
        return;
    }

    node->expression->Accept(visitor);

    auto result = visitor->GetResult();
    context->handleReturn(node, result.type);

    visitor->AddSuccess();
}
