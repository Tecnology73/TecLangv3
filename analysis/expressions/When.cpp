#include "When.h"
#include <set>
#include <utility>
#include "../topLevel/Enum.h"
#include "../../ast/expressions/StaticRef.h"
#include "../../ast/literals/Integer.h"
#include "../../compiler/TypeCoercion.h"
#include "../../scope/Scope.h"

void WhenAnalyzer::Analyze() {
    // Create a context
    auto [scope, context] = Scope::Enter<WhenAnalysisContext>(visitor, node);

    // Visit everything in the body
    for (const auto& item: node->body) {
        item->Accept(visitor);

        if (VisitorResult result; !visitor->TryGetResult(result)) return;
    }

    // Run checks
    exhaustiveCaseCheck();
    inferReturnTypes(context);

    // Cleanup
    scope->Leave();

    if (node->returnType)
        visitor->AddSuccess(node->returnType);
}

bool WhenAnalyzer::exhaustiveCaseCheck() {
    // We don't need to do exhaustive case checking here when an `else` case is specified.
    if (node->hasElse) return true;

    // Collect all the values being used.
    std::set<std::string_view> caseValues;
    for (const auto& item: node->body) {
        if (!item->condition)
            throw std::runtime_error("`hasElse` returned false but there is an expression-less case.");

        if (auto staticRef = static_cast<StaticRef*>(item->condition))
            caseValues.emplace(staticRef->next->name);
        else
            throw std::runtime_error("Unhandled expression expression.");
    }

    // Check for Enum values that haven't been used.
    std::vector<std::string_view> missingValues;
    for (const auto& fieldName: EnumAnalyzer::lastEnum->fieldOrder) {
        if (caseValues.contains(fieldName)) continue;

        missingValues.push_back(fieldName);
    }

    // All have been used.
    if (missingValues.empty()) return true;

    // Build up the hint message.
    std::string missingValuesStr;
    constexpr int MAX_MISSING_VALUES = 2;
    int max = std::min(static_cast<int>(missingValues.size()), MAX_MISSING_VALUES);
    for (const auto& item: missingValues) {
        if (max-- == 0) break;
        missingValuesStr += "- {Y}" + EnumAnalyzer::lastEnum->name + "{reset}::" + item.data();
        if (max > 0 && missingValues.size() - max > 0) missingValuesStr += "\n";
    }

    if (missingValues.size() > MAX_MISSING_VALUES)
        missingValuesStr += "\n+ " + std::to_string(missingValues.size() - MAX_MISSING_VALUES) + " more...";

    ErrorManager::QueueHint("Add an 'else' or provide the missing values:\n" + missingValuesStr, {});
    visitor->ReportError(ErrorCode::WHEN_UNUSED_ENUM_VALUE, {}, node);
    return false;
}

void WhenAnalyzer::inferReturnTypes(std::shared_ptr<WhenAnalysisContext> context) {
    // Collect all the return types seen
    /*std::set<const TypeVariant *> returnTypes;
    for (const auto &key: context->returnStatements | std::views::keys)
        returnTypes.emplace(key);

    // Reduce all common types
    reduceReturnTypes(returnTypes);

    if (returnTypes.empty()) {
        // If there are no return statements,
        // it should be safe to assume that the return type is void.
        node->returnType = SymbolTable::GetInstance()->GetVariant<TypeDefinition>("void");
    } else if (returnTypes.size() != 1) {
        visitor->ReportError(ErrorCode::WHEN_MULTIPLE_RETURN_TYPES, {}, node);
    } else if (!node->returnType) {
        node->returnType = const_cast<TypeVariant *>(*returnTypes.begin());
    }*/
}

/// <summary>
/// Try to determine a common type from a set of types.
/// For example, if a bool & i8 are returned, the common type would be i8.
/// The goal is to reduce the number of return types to 1.
/// </summary>
void WhenAnalyzer::reduceReturnTypes(std::set<TypeReference*>& types) {
    if (types.size() <= 1) return;

    std::vector<TypeReference*> typesArray;
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
