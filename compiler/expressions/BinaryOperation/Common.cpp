#include "Common.h"
#include "../../Compiler.h"

llvm::Value* BinaryOperation_Common::generateValue(
    Visitor* v,
    const BinaryOperation* node,
    Node* valueNode,
    bool ensureLoaded
) {
    // Generate the value
    valueNode->Accept(v);

    VisitorResult result;
    if (!v->TryGetResult(result))
        return nullptr;

    // We don't load value types.
    if (!result.value->getType()->isPointerTy()) {
        v->AddSuccess(result.value, result.type);
        return result.value;
    }

    // We don't load if we're doing an assign op.
    if (node->op == Token::Type::Assign || node->op == Token::Type::PlusEqual) {
        // I don't know if this is the best thing to do here.
        // I think we need to load for value types, but not for objects?
        // If we have a local variable allocated, we need it to be loaded before we store it in another variable.
        // newLength = this.length + strlen("hello");
        // this.length = newLength; // newLength must be loaded.
        if (ensureLoaded && result.type && result.type->ResolveType()->isValueType)
            result.value = Compiler::getBuilder().CreateLoad(result.type->ResolveType()->getLlvmType(), result.value);

        v->AddSuccess(result.value, result.type);
        return result.value;
    }

    if (auto cNode = dynamic_cast<ChainableNode *>(valueNode)) {
        v->AddSuccess(result.value, result.type);
        return Compiler::getBuilder().CreateLoad(cNode->getFinalType()->getLlvmType(), result.value);
    }

    v->AddSuccess(result.value, result.type); //
    return result.value;
}
