#include "Expressions.h"
#include "Statements.h"

ConstructorCall *ConstructorCall::Create(const Token &token) {
    auto node = new ConstructorCall(token);
    node->type = TypeDefinition::Create(token, token.value);

    return node;
}

Position ConstructorCall::GetEndPosition() const {
    if (!arguments.empty()) {
        return arguments.back()->token.position;
    } else if (!assignments.empty()) {
        return assignments.back()->token.position;
    }

    return token.position;
}

BinaryOperation *BinaryOperation::Create(const Token &token, Node *lhs, Node *rhs) {
    auto node = new BinaryOperation(token);
    node->lhs = lhs;
    node->rhs = rhs;
    node->op = token.type;

    return node;
}
