#include "ConstructorCall.h"
#include "../StringInternTable.h"

Position ConstructorCall::GetEndPosition() const {
    if (!arguments.empty())
        return arguments.back()->token.position;
    if (!fields.empty())
        return fields.back()->token.position;

    return token.position;
}

void ConstructorCall::AddArgument(Node* arg) {
    arguments.emplace_back(arg);
}

void ConstructorCall::AddField(const Token& token, Node* value) {
    fields.emplace_back(
        new ConstructorField(token, StringInternTable::Intern(token.value), value)
    );
}
