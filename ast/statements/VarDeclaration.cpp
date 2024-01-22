#include "VarDeclaration.h"
#include "../../compiler/Compiler.h"
#include "../../compiler/statements/TypeDefinition.h"

VariableDeclaration::VariableDeclaration(const Token& beginToken, Node* expression) : Node(beginToken),
    name(token.value),
    expression(expression) {
}

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

/*TypeVariant* VariableDeclaration::GetCompiledType(Visitor* visitor) {
    if (!type) {
        type = inferType(visitor, this)->CreateReference();
        if (!type) return nullptr;
    }

    if (!type->ResolveType()->type->llvmType) {
        if (auto typeDef = dynamic_cast<TypeDefinition *>(type->type))
            generateTypeDefinition(visitor, typeDef);
        else
            type->type->llvmType = Compiler::getScopeManager().getType("i32")->llvmType;
    }

    return type;
}*/
