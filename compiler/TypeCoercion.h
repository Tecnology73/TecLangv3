#pragma once

#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>
#include "Compiler.h"

class TypeCoercion {
public:
    static std::pair<llvm::Value *, llvm::Value *> coerce(llvm::Value *valueA, llvm::Value *valueB);

    static llvm::Value *coerce(llvm::Value *value, llvm::Type *toType);

    static bool canCoerceTo(TypeBase *fromType, TypeBase *toType);

private:
    enum class Priority {
        None = 0,
        Integer,
        Double,
    };

    static int getTypePriority(llvm::Type *type);

    static llvm::Value *coerceToInt(llvm::Value *value, llvm::Type *type);

    static llvm::Value *coerceToDouble(llvm::Value *value, llvm::Type *type);

    static unsigned getBitWidth(llvm::Type *type);
};