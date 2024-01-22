#pragma once

#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>
#include "Compiler.h"

class TypeCoercion {
public:
    static std::pair<llvm::Value *, llvm::Value *> coerce(llvm::Value* valueA, llvm::Value* valueB);

    static llvm::Value* coerce(llvm::Value* value, llvm::Type* toType, bool forcePointerCast = false);

    static bool isTypeCompatible(const TypeVariant* type, const TypeVariant* otherType);

    static bool isTypeCompatible(const TypeBase* type, const TypeBase* otherType);

    static TypeBase* getCommonType(const TypeVariant* typeA, const TypeVariant* typeB);

    static TypeBase* getCommonType(TypeBase* typeA, TypeBase* typeB);

    static llvm::Type* getCommonType(llvm::Type* typeA, llvm::Type* typeB);

    static bool canCoerceTo(const TypeBase* fromType, const TypeBase* toType);

private:
    enum class Priority {
        None = 0,
        Boolean,
        Int8,
        Int16,
        Int32,
        Int64,
        Double,
    };

    static int getTypePriority(const llvm::Type* type);

    static int getTypePriority(const TypeBase* type);

    static llvm::Value* coerceToInt(llvm::Value* value, llvm::Type* type);

    static llvm::Value* coerceToDouble(llvm::Value* value, llvm::Type* type);

    static unsigned getBitWidth(llvm::Type* type);
};
