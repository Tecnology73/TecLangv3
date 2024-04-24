#include "StaticRef.h"
#include "../../symbolTable/SymbolTable.h"
#include "../../scope/Scope.h"

void StaticRefAnalyzer::Analyze() {
    // Create a context
    auto [scope, context] = Scope::Enter<StaticRefAnalysisContext>(visitor, node);

    // TODO: The only static thing we have are Enum's atm.
    context->anEnum = SymbolTable::GetInstance()->Get<Enum>(node->name);
    if (!context->anEnum) {
        visitor->ReportError(ErrorCode::STATIC_REF_UNKNOWN, {node->name}, node);
        return;
    }

    if (auto field = dynamic_cast<VariableReference*>(node->next))
        AnalyzeField(context, field);
    else if (auto fnCall = dynamic_cast<FunctionCall*>(node->next))
        AnalyzeConstructor(context, fnCall);

    // VisitorResult result;
    // if (!visitor->TryGetResult(result)) return;

    // Cleanup
    scope->Leave();

    // visitor->AddSuccess(result.type);
}

void StaticRefAnalyzer::AnalyzeField(std::shared_ptr<StaticRefAnalysisContext> context, VariableReference* ref) {
    auto field = context->anEnum->GetField(node->next->name);
    if (!field) {
        visitor->ReportError(ErrorCode::ENUM_UNKNOWN_VALUE, {ref->name, context->anEnum->name}, ref);
        return;
    }

    field->Accept(visitor);
}

void StaticRefAnalyzer::AnalyzeConstructor(std::shared_ptr<StaticRefAnalysisContext> context, FunctionCall* fnCall) {
    auto fn = context->anEnum->GetFunction(fnCall->name);
    if (!fn) {
        visitor->ReportError(ErrorCode::ENUM_UNKNOWN_VALUE, {fnCall->name, context->anEnum->name}, fnCall);
        return;
    }

    fn->Accept(visitor);
}
