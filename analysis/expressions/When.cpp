#include "When.h"
#include <set>
#include <utility>
#include "../topLevel/Enum.h"
#include "../../ast/expressions/StaticRef.h"
#include "../../ast/literals/Integer.h"
#include "../../compiler/Compiler.h"
#include "../../compiler/TypeCoercion.h"

void WhenAnalyzer::Analyze() {
    // Create a context
    auto context = Compiler::getScopeManager().enter("when", new WhenAnalysisContext(visitor, node));

    // Visit everything in the body
    for (const auto& item: node->body) {
        item->Accept(visitor);

        if (VisitorResult result; !visitor->TryGetResult(result)) return;
    }

    // Run checks
    exhaustiveCaseCheck();
    inferReturnTypes(context);

    // Cleanup
    Compiler::getScopeManager().popContext();
    Compiler::getScopeManager().leave("when");

    if (node->returnType)
        visitor->AddSuccess(node->returnType);
}

bool WhenAnalyzer::exhaustiveCaseCheck() {
    // We don't need to do exhaustive case checking here when an `else` case is specified.
    if (node->hasElse) return true;

    // Collect all the values being used.
    std::set<std::string> caseValues;
    for (const auto& item: node->body) {
        if (!item->condition)
            throw std::runtime_error("`hasElse` returned false but there is an expression-less case.");

        if (auto staticRef = static_cast<StaticRef *>(item->condition))
            caseValues.emplace(staticRef->next->name);
        else
            throw std::runtime_error("Unhandled expression expression.");
    }

    // Check for Enum values that haven't been used.
    std::vector<std::string> missingValues;
    for (const auto& fieldName: EnumAnalyzer::lastEnum->fieldOrder) {
        if (caseValues.contains(fieldName)) continue;

        missingValues.push_back(fieldName);
    }

    // All have been used.
    if (missingValues.empty()) return true;

    // Build up the hint message.
    std::string missingValuesStr;
    const int MAX_MISSING_VALUES = 2;
    int max = std::min((int) missingValues.size(), MAX_MISSING_VALUES);
    for (const auto& item: missingValues) {
        if (max-- == 0) break;
        missingValuesStr += "- {Y}" + EnumAnalyzer::lastEnum->name + "{reset}::" + item;
        if (max > 0 && missingValues.size() - max > 0) missingValuesStr += "\n";
    }

    if (missingValues.size() > MAX_MISSING_VALUES)
        missingValuesStr += "\n+ " + std::to_string(missingValues.size() - MAX_MISSING_VALUES) + " more...";

    ErrorManager::QueueHint("Add an 'else' or provide the missing values:\n" + missingValuesStr, {});
    visitor->ReportError(ErrorCode::WHEN_UNUSED_ENUM_VALUE, {}, node);
    return false;
}

void WhenAnalyzer::inferReturnTypes(const WhenAnalysisContext* context) {
    // Collect all of the return types seen
    std::set<const TypeVariant *> returnTypes;
    for (const auto& key: context->returnStatements | std::views::keys)
        returnTypes.emplace(key);

    // Reduce all common types
    reduceReturnTypes(returnTypes);

    if (returnTypes.empty()) {
        // If there are no return statements,
        // it should be safe to assume that the return type is void.
        node->returnType = Compiler::getScopeManager().getType("void")->createVariant();
    } else if (returnTypes.size() != 1) {
        visitor->ReportError(ErrorCode::WHEN_MULTIPLE_RETURN_TYPES, {}, node);
    } else if (!node->returnType) {
        node->returnType = const_cast<TypeVariant *>(*returnTypes.begin());
    }
}

/// <summary>
/// Try to determine a common type from a set of types.
/// For example, if a bool & i8 are returned, the common type would be i8.
/// The goal is to reduce the number of return types to 1.
/// </summary>
void WhenAnalyzer::reduceReturnTypes(std::set<const TypeVariant *>& types) {
    if (types.size() <= 1) return;

    std::vector<const TypeVariant *> typesArray;
    typesArray.reserve(types.size());
    for (const auto& item: types)
        typesArray.emplace_back(item);

    // Compare each type to the next to see if they can be coerced
    for (size_t i = 0; i < typesArray.size() - 1; i++) {
        auto type = typesArray[i];
        auto nextType = typesArray[i + 1];

        // If the next type can be coerced to the current type, remove it from the set
        if (TypeCoercion::isTypeCompatible(nextType, type)) {
            types.erase(nextType);
            typesArray.erase(typesArray.begin() + i + 1);
            i--;
        } else if (TypeCoercion::isTypeCompatible(type, nextType)) {
            types.erase(type);
            typesArray.erase(typesArray.begin() + i);
            i--;
        }
    }
}
