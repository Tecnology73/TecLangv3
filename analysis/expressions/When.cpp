#include "When.h"
#include <set>
#include <unordered_map>
#include <utility>
#include "../topLevel/Enum.h"
#include "../../ast/expressions/StaticRef.h"
#include "../../ast/literals/Integer.h"
#include "../../compiler/Compiler.h"

void WhenAnalyzer::Analyze() {
    exhaustiveCaseCheck();
    inferReturnType();
}

bool WhenAnalyzer::exhaustiveCaseCheck() {
    // We don't need to do exhaustive case checking here when an `else` case is specified.
    if (node->hasElse) return true;

    // Collect all the values being used.
    std::set<std::string> caseValues;
    for (const auto &item: node->body) {
        if (!item->condition)
            throw std::runtime_error("`hasElse` returned false but there is a expression-less case.");

        if (auto staticRef = static_cast<StaticRef *>(item->condition))
            caseValues.emplace(staticRef->next->name);
        else
            throw std::runtime_error("Unhandled expression expression.");
    }

    // Check for Enum values that haven't been used.
    std::vector<std::string> missingValues;
    for (const auto &fieldName: EnumAnalyzer::lastEnum->fieldOrder) {
        if (caseValues.contains(fieldName)) continue;

        missingValues.push_back(fieldName);
    }

    // All have been used.
    if (missingValues.empty()) return true;

    // Build up the hint message.
    std::string missingValuesStr;
    const int MAX_MISSING_VALUES = 2;
    int max = std::min((int) missingValues.size(), MAX_MISSING_VALUES);
    for (const auto &item: missingValues) {
        if (max-- == 0) break;
        missingValuesStr += "- {Y}" + EnumAnalyzer::lastEnum->name + "{reset}::" + item;
        if (max > 0 && missingValues.size() - max > 0) missingValuesStr += "\n";
    }

    if (missingValues.size() > MAX_MISSING_VALUES)
        missingValuesStr += "\n+ " + std::to_string(missingValues.size() - MAX_MISSING_VALUES) + " more...";

    ErrorManager::QueueHint("AddField an 'else' or provide the missing values:\n" + missingValuesStr, {});
    visitor->ReportError(ErrorCode::WHEN_UNUSED_ENUM_VALUE, {}, node);
    return false;
}

bool WhenAnalyzer::inferReturnType() {
    std::set<TypeBase *> returnTypes;
    for (const auto &item: node->body) {
        if (!item->implicitReturn) continue;

        auto value = static_cast<Integer *>(item->body[0]);
        if (!value) continue;

        returnTypes.emplace(
            Compiler::getScopeManager().getType("i" + std::to_string(value->numBits))
        );
    }

    if (returnTypes.empty()) {
        std::cout << "No return types found." << std::endl;
        return true;
    }

    if (returnTypes.size() == 1) {
        std::cout << "Return type: " << (*returnTypes.begin())->name << std::endl;
        return true;
    }

    visitor->ReportError(ErrorCode::UNKNOWN_ERROR, {}, node);
    return false;
}
