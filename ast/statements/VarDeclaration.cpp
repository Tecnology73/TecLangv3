#include "VarDeclaration.h"

VariableDeclaration::VariableDeclaration(
    const Token& beginToken,
    const std::string& name,
    Node* expression
) : Node(beginToken),
    name(name),
    expression(expression) {
}

VariableDeclaration::VariableDeclaration(
    const Token& beginToken,
    const std::string& name,
    TypeReference* type,
    Node* expression
) : Node(beginToken),
    name(name),
    type(type),
    expression(expression) {
}

VariableDeclaration::~VariableDeclaration() {
    delete expression;
}

void VariableDeclaration::Accept(Visitor* visitor) {
    visitor->Visit(this);
}
