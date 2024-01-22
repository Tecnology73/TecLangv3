#include "Common.h"
#include "../../Compiler.h"

llvm::Value* BinaryOperation_Common::generateValue(Visitor* v, const BinaryOperation* node, Node* valueNode) {
    // Generate the value
    valueNode->Accept(v);

    VisitorResult result;
    if (!v->TryGetResult(result))
        return nullptr;

    // We don't load value types.
    if (!result.value->getType()->isPointerTy())
        return result.value;

    // We don't load if we're doing an assign op.
    if (node->op == Token::Type::Assign || node->op == Token::Type::PlusEqual)
        return result.value;

    if (auto cNode = dynamic_cast<ChainableNode *>(valueNode))
        return Compiler::getBuilder().CreateLoad(cNode->getFinalType()->type->getLlvmType(), result.value);

    // wtf happened??
    return nullptr;
}
