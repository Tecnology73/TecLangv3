#include "IfStatement.h"
#include "../../compiler/Compiler.h"

void IfStatementAnalyzer::Analyze() {
    auto context = Compiler::getScopeManager().enter("if", new IfStatementAnalysisContext(visitor, node));

    if (analyzeCondition() && analyzeBody(context) && analyzeElse()) {
        visitor->AddSuccess();

        if (context->hasReturned)
            propagateNarrowedTypes();
    }

    Compiler::getScopeManager().popContext();
    Compiler::getScopeManager().leave("if");
}

bool IfStatementAnalyzer::analyzeCondition() {
    // No condition for an if statement is invalid but this should be caught by the parser.
    // Otherwise, no condition means this is an else statement.
    if (!node->condition) return true;

    node->condition->Accept(visitor);

    VisitorResult result;
    return visitor->TryGetResult(result);
}

bool IfStatementAnalyzer::analyzeBody(IfStatementAnalysisContext* context) {
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

void IfStatementAnalyzer::propagateNarrowedTypes() {
    auto parentScope = Compiler::getScopeManager().GetParentScope();
    for (const auto& [name, symbol]: Compiler::getScopeManager().GetVars()) {
        auto typeInParent = parentScope->GetVar(name)->narrowedType;
        auto type = symbol->narrowedType;

        if (type->type->name == "null" && typeInParent->flags.Has(TypeFlag::OPTIONAL)) // is null
            typeInParent->flags.Clear(TypeFlag::OPTIONAL);
        else if (!type->type->isValueType && typeInParent->flags.Has(TypeFlag::OPTIONAL)) // is Type
            parentScope->Add(name, symbol->node, Compiler::getScopeManager().getType("null")->createVariant());
    }
}
