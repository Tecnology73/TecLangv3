#include "FunctionCall.h"
#include "../../compiler/Compiler.h"
#include "../../context/preAnalysis/FunctionContext.h"
#include "../../symbolTable/SymbolTable.h"

void analyseFunctionCall(Visitor* visitor, FunctionCall* node) {
    std::vector<TypeReference *> args;
    args.reserve(node->arguments.size());
    for (const auto& arg: node->arguments) {
        arg->Accept(visitor);

        VisitorResult result;
        if (!visitor->TryGetResult(result)) return;

        args.push_back(result.type);
    }

    node->function = SymbolTable::GetInstance()->LookupFunction(node->name, args);
    if (!node->function || !node->function->returnType) {
        visitor->AddFailure();
        return;
    }

    visitor->AddSuccess(node->function->returnType);
}
