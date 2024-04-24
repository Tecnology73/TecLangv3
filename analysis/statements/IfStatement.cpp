#include "IfStatement.h"
#include "../../symbolTable/SymbolTable.h"
#include "../../scope/Scope.h"

void IfStatementAnalyzer::Analyze() {
    auto [scope, context] = Scope::Enter<IfStatementAnalysisContext>(visitor, node);

    if (analyzeCondition() && analyzeBody(context) && analyzeElse()) {
        visitor->AddSuccess();

        if (context->hasReturned)
            propagateNarrowedTypes(scope);
    }

    scope->Leave();
}

bool IfStatementAnalyzer::analyzeCondition() {
    // No condition for an if statement is invalid but this should be caught by the parser.
    // Otherwise, no condition means this is an else statement.
    if (!node->condition) return true;

    node->condition->Accept(visitor);

    VisitorResult result;
    return visitor->TryGetResult(result);
}

bool IfStatementAnalyzer::analyzeBody(std::shared_ptr<IfStatementAnalysisContext> context) {
    while (const auto& item = context->GetNextNode()) {
        item->Accept(visitor);

        if (VisitorResult result; !visitor->TryGetResult(result)) return false;
    }

    return true;
}

bool IfStatementAnalyzer::analyzeElse() {
    if (!node->elseStatement) return true;

    node->elseStatement->Accept(visitor);

    VisitorResult result;
    return visitor->TryGetResult(result);
}

void IfStatementAnalyzer::propagateNarrowedTypes(std::shared_ptr<Scope> scope) {
    /*auto parentScope = scope->GetParent();
    for (const auto& [name, symbol]: scope->List()) {
        auto typeInParent = parentScope->Get(name)->type;
        auto type = symbol->type;

        if (type->name == "null" && typeInParent->flags.Has(TypeFlag::OPTIONAL)) // is null
            typeInParent->flags.Clear(TypeFlag::OPTIONAL);
        else if (!type->ResolveType()->isValueType && typeInParent->flags.Has(TypeFlag::OPTIONAL)) // is Type
            parentScope->Add(name, symbol->node, SymbolTable::GetInstance()->GetReference("null"));
    }*/
}
