#include "BinaryOperation.h"
#include "./BinaryOperation/Null.h"
#include "./BinaryOperation/Value.h"

void generateBinaryOperation(Visitor* v, const BinaryOperation* node) {
    llvm::Value* value;
    switch (node->op) {
        case Token::Type::Equal:
            value = BinaryOperation_Null::generateEqual(v, node);
            break;
        case Token::Type::NotEqual:
            value = BinaryOperation_Null::generateNotEqual(v, node);
            break;
        case Token::Type::Assign:
            value = BinaryOperation_Null::generateAssign(v, node);
            break;
        case Token::Type::Is:
            value = BinaryOperation_Null::generateIs(v, node);
            break;
        default:
            BinaryOperation_Value::generate(v, node);
            return;
    }

    if (value == nullptr) {
        v->ReportError(ErrorCode::UNKNOWN_ERROR, {}, node);
        return;
    }

    v->AddSuccess(value);
}
