#pragma once

#include "TypeBase.h"
#include "../Expressions.h"

class TypeDefinition : public TypeBase {
public:
    using TypeBase::TypeBase;

    std::map<std::string, VariableDeclaration *> fields;
    std::map<std::string, int> fieldIndices;
    std::map<std::string, std::vector<Function *>> functions;

    llvm::Value *Accept(class Visitor *visitor) override {
        visitor->Visit(this);
        return nullptr;
    }

    llvm::Value *getDefaultValue() const override;

    bool canCastTo(TypeBase *other) const override;

    llvm::Type *setLlvmType(llvm::Type *type);

    void addField(VariableDeclaration *field);

    void addFunction(Function *function);

    VariableDeclaration *getField(const std::string &fieldName) const;

    int getFieldIndex(const std::string &fieldName);

    TypeDefinition *getFieldTypeDef(const std::string &fieldName) const;

    Function *getFunction(const std::string &name) const;

    Function *getFunction(const std::string &funcName, const std::vector<llvm::Value *> &parameters) const;

    static TypeDefinition *Create(const Token &token);

    static TypeDefinition *Create(const Token &token, std::string name);
};