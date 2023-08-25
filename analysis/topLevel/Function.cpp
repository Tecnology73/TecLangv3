#include "Function.h"

void FunctionAnalyzer::Analyze() {
    for (const auto &item: node->body)
        item->Accept(visitor);

    // TODO: Infer return type
    /*std::set<TypeDefinition *> returnTypes;
    for (const auto &item: context->returnStatements) {
        auto type = Compiler::getScopeManager().getLlvmType(item.first);
        if (!type) {
            v->ReportError(ErrorCode::UNKNOWN_ERROR, {}, item.second[0]);
            continue;
        }

        returnTypes.emplace(type);
    }

    if (returnTypes.size() != 1) {
        v->ReportError(ErrorCode::UNKNOWN_ERROR, {}, func);
        return nullptr;
    }*/
}
