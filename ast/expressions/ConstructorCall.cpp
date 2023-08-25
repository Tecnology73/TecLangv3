#include "ConstructorCall.h"

Position ConstructorCall::GetEndPosition() const {
    if (!arguments.empty())
        return arguments.back()->token.position;
    else if (!fields.empty())
        return fields.back()->token.position;

    return token.position;
}

void ConstructorCall::AddArgument(Node *arg) {
    arguments.emplace_back(arg);
}

void ConstructorCall::AddField(Token token, Node *value) {
    fields.emplace_back(new ConstructorField(std::move(token), value));
}

ConstructorCall *ConstructorCall::Create(const Token &token) {
    auto node = new ConstructorCall(token);
    // node->type = TypeDefinition::Create(token, token.expression);

    return node;
}
