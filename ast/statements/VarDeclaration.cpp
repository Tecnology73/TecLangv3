#include "VarDeclaration.h"
#include "../../compiler/Compiler.h"
#include "../../compiler/statements/TypeDefinition.h"

VariableDeclaration::VariableDeclaration(const Token &beginToken, Node *expression) :
    Node(beginToken),
    name(token.value),
    expression(expression) {}

VariableDeclaration::VariableDeclaration(const Token &beginToken, const std::string &name, Node *expression) :
    Node(beginToken),
    name(name),
    expression(expression) {}

VariableDeclaration::~VariableDeclaration() {
    delete expression;
}

llvm::Value *VariableDeclaration::Accept(Visitor *visitor) {
    return visitor->Visit(this);
}

TypeBase *VariableDeclaration::GetCompiledType(Visitor *visitor) {
    if (!type) {
        type = inferType(visitor, this);
        if (!type) return nullptr;
    }

    if (!type->llvmType) {
        if (auto typeDef = dynamic_cast<TypeDefinition *>(type))
            generateTypeDefinition(visitor, typeDef);
        else
            type->llvmType = Compiler::getScopeManager().getType("i32")->llvmType;
    }

    return type;
}
