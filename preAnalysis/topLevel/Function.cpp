#include "Function.h"
#include "../../symbolTable/SymbolTable.h"
#include "../../context/preAnalysis/FunctionContext.h"
#include "../../scope/Scope.h"

void analyseFunction(Visitor* visitor, Function* node) {
    if (node->analysisInfo) {
        // This function has already been analysed.
        visitor->AddSuccess();
        return;
    }

    node->analysisInfo = new AnalysisInfo(node);
    auto [scope, context] = Scope::Enter<FunctionContext>(visitor, node);

    while (const auto item = context->GetNextNode()) {
        item->Accept(visitor);
        // We don't actually care about the result at this stage,
        // but we still need to consume it.
        visitor->GetResult();
    }

    // If there are no unknown return types, we can try to resolve the return type now.
    if (!node->returnType && node->analysisInfo->unknownReturnTypes.empty()) {
        if (node->analysisInfo->possibleReturnTypes.empty())
            node->returnType = SymbolTable::GetInstance()->GetReference("void");
        else if (node->analysisInfo->possibleReturnTypes.size() == 1) {
            node->returnType = SymbolTable::GetInstance()->GetReference(
                std::string(node->analysisInfo->possibleReturnTypes.begin()->first)
            );

            node->analysisInfo->possibleReturnTypes.clear();
        } else {
            scope->Leave();

            visitor->ReportError(ErrorCode::FUNCTION_MULTIPLE_RETURN_TYPES, {node->name}, node);
            return;
        }
    }

    scope->Leave();

    visitor->AddSuccess();
}
