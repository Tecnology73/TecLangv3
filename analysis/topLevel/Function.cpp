#include "Function.h"
#include "../../compiler/Compiler.h"
#include "../../compiler/TypeCoercion.h"
#include "../../symbolTable/SymbolTable.h"

void FunctionAnalyzer::Analyze() {
    if (node->isExternal) {
        Compiler::getScopeManager().add(node);
        return;
    }

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

    // Register the function
    if (!node->ownerType)
        Compiler::getScopeManager().add(node);

    visitor->AddSuccess(node->returnType->ResolveType());
}

bool FunctionAnalyzer::inferReturnTypes(const FunctionAnalysisContext* context) {
    // Collect all of the return types seen
    std::set<const TypeVariant *> returnTypes;
    for (const auto& key: context->returnStatements | std::views::keys)
        returnTypes.emplace(key);

    // Reduce all common types
    reduceReturnTypes(returnTypes);

    if (returnTypes.empty()) {
        // If there are no return statements and a return type hasn't been specified,
        // it should be safe to assume that the return type is void.
        if (!node->returnType)
            node->returnType = std::get<TypeDefinition *>(
                SymbolTable::GetInstance()->Get("void")->value
            )->CreateReference();
    } else if (returnTypes.size() != 1) {
        visitor->ReportError(ErrorCode::FUNCTION_MULTIPLE_RETURN_TYPES, {std::string(node->name)}, node);
        return false;
    } else if (!node->returnType) {
        node->returnType = (*returnTypes.begin())->CreateReference();
    }

    if (node->returnType->name == "void" && context->returnStatements.empty())
        node->body.push_back(new Return(Token{}));

    return true;
}

/// <summary>
/// Try to determine a common type from a set of types.
/// For example, if a bool & i8 are returned, the common type would be i8.
/// The goal is to reduce the number of return types to 1.
/// </summary>
void FunctionAnalyzer::reduceReturnTypes(std::set<const TypeVariant *>& types) {
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
