#include "Function.h"
#include "../../compiler/Compiler.h"
#include "../../symbolTable/SymbolTable.h"
#include "../../context/preAnalysis/FunctionContext.h"

void analyseFunction(Visitor* visitor, Function* node) {
    if (node->analysisInfo) {
        // This function has already been analysed.
        visitor->AddSuccess();
        return;
    }

    node->analysisInfo = new AnalysisInfo(node);
    auto context = Compiler::getScopeManager().enter(node->name, new FunctionContext(visitor, node));

    while (const auto item = context->GetNextNode()) {
        item->Accept(visitor);
        // We don't actually care about the result at this stage
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
            Compiler::getScopeManager().popContext();
            Compiler::getScopeManager().leave(node->name);

            visitor->ReportError(ErrorCode::FUNCTION_MULTIPLE_RETURN_TYPES, {node->name}, node);
            return;
        }
    }

    Compiler::getScopeManager().popContext();
    Compiler::getScopeManager().leave(node->name);

    visitor->AddSuccess();
}
