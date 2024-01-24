#pragma once

#include <llvm/IR/GlobalVariable.h>
#include "../Node.h"
#include "../Visitor.h"
#include "../topLevel/TypeDefinition.h"
#include "../../symbolTable/SymbolTable.h"

class String : public Literal {
public:
    String(const Token& token, const std::string& value)
        : Literal(token),
          value(value) {
    }

    void Accept(class Visitor* visitor) override {
        visitor->Visit(this);
    }

    TypeVariant* getType() override {
        return std::get<TypeDefinition *>(SymbolTable::GetInstance()->Get("string")->value)->createVariant();
    }

public:
    const std::string& value;

    llvm::GlobalVariable* llvmValue = nullptr;
};
