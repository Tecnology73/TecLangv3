#include "Function.h"
#include "../../compiler/Compiler.h"
#include "../../compiler/TypeCoercion.h"
#include "../../symbolTable/SymbolTable.h"

void FunctionAnalyzer::Analyze() {
    if (node->isExternal)
        return;

    if (node->ownerType && !node->ownerType->isDeclared) {
        visitor->ReportError(ErrorCode::TYPE_UNKNOWN, {node->ownerType->name}, node->ownerType);
        return;
    }

    // Create a context
    auto context = Compiler::getScopeManager().enter(
        std::string(node->name),
        new FunctionAnalysisContext(visitor, node)
    );

    // Visit all the parameters
    for (const auto& param: node->parameters | std::views::values) {
        param->Accept(visitor);

        if (VisitorResult result; !visitor->TryGetResult(result)) {
            if (result.errorCode != ErrorCode::VALUE_CANNOT_BE_NULL)
                return;
        }
    }

    // Visit everything in the body
    while (const auto& item = context->GetNextNode()) {
        item->Accept(visitor);

        if (VisitorResult result; !visitor->TryGetResult(result)) {
            if (result.errorCode != ErrorCode::VALUE_CANNOT_BE_NULL)
                return;
        }
    }

    if (!inferReturnTypes(context))
        return;

    // Cleanup
    Compiler::getScopeManager().popContext();
    Compiler::getScopeManager().leave(std::string(node->name));

    visitor->AddSuccess(node->returnType);
}

bool FunctionAnalyzer::inferReturnTypes(const FunctionAnalysisContext* context) {
    if (node->returnType)
        return true;

    while (!node->analysisInfo->unknownReturnTypes.empty()) {
        auto unknown = node->analysisInfo->unknownReturnTypes.top();

        // Try to resolve the type.
        unknown->Accept(visitor);

        VisitorResult result;
        if (!visitor->TryGetResult(result)) return false;

        node->analysisInfo->AddReturnType(node, result.type);

        // We don't pop it until here so that we can maybe
        // use it to show a more detailed error message.
        node->analysisInfo->unknownReturnTypes.pop();
    }

    if (!node->analysisInfo->unknownReturnTypes.empty()) {
        // Change error to "unable to infer return type".
        visitor->ReportError(ErrorCode::FUNCTION_MULTIPLE_RETURN_TYPES, {node->name}, node);
        return false;
    }

    if (node->analysisInfo->possibleReturnTypes.size() > 1) {
        visitor->ReportError(ErrorCode::FUNCTION_MULTIPLE_RETURN_TYPES, {node->name}, node);
        return false;
    }

    node->returnType = SymbolTable::GetInstance()->GetReference(
        std::string(node->analysisInfo->possibleReturnTypes.begin()->first)
    );
    return true;
}

/// <summary>
/// Try to determine a common type from a set of types.
/// For example, if a bool & i8 are returned, the common type would be i8.
/// The goal is to reduce the number of return types to 1.
/// </summary>
void FunctionAnalyzer::reduceReturnTypes(std::set<TypeReference *>& types) {
    if (types.size() <= 1) return;

    std::vector<TypeReference *> typesArray;
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
