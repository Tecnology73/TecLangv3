#include "ReturnContext.h"
#include "FunctionContext.h"

TypeDefinition *ReturnContext::getReturnType() {
    if (auto functionContext = dynamic_cast<FunctionContext *>(parent))
        return functionContext->getReturnType();

    return nullptr;
}

void ReturnContext::handleReturn(const Node *node, llvm::Value *value) {
    Compiler::getBuilder().CreateRet(value);

    if (auto functionContext = dynamic_cast<FunctionContext *>(parent))
        functionContext->addReturnType(node, value);
}
