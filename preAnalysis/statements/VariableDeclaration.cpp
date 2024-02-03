#include "VariableDeclaration.h"
#include "../../context/preAnalysis/FunctionContext.h"

void analyseVariableDeclaration(Visitor* visitor, VariableDeclaration* node) {
    if (!node->expression) {
        visitor->AddSuccess();
        return;
    }

    node->expression->Accept(visitor);
    visitor->GetResult();

    visitor->AddSuccess();
}
