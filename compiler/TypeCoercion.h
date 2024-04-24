#pragma once

#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>
#include "../ast/expressions/TypeReference.h"

class TypeCoercion {
public:
    static std::pair<llvm::Value*, llvm::Value*> coerce(llvm::Value* valueA, llvm::Value* valueB);

    static llvm::Value* coerce(llvm::Value* value, llvm::Type* toType, bool forcePointerCast = false);

    static bool isTypeCompatible(TypeReference* type, TypeReference* otherType);

    static bool isTypeCompatible(const TypeBase* type, const TypeBase* otherType);

    static TypeReference* getCommonType(TypeReference* typeA, TypeReference* typeB);

    static TypeBase* getCommonType(TypeBase* typeA, TypeBase* typeB);

    static llvm::Type* getCommonType(llvm::Type* typeA, llvm::Type* typeB);

    static bool canCoerceTo(const TypeBase* fromType, const TypeBase* toType);

private:
    // Items placed higher in this list will allow for lower items to be coerced up to them.
    enum class Priority {
        None = 0,
        Boolean,
        Int8,
        UInt8,
        Int16,
        UInt16,
        Int32,
        UInt32,
        Int64,
        UInt64,
        Double,
    };

    static int getTypePriority(const llvm::Type* type);

    static int getTypePriority(const TypeBase* type);

    static llvm::Value* coerceToInt(llvm::Value* value, llvm::Type* type);

    static llvm::Value* coerceToDouble(llvm::Value* value, llvm::Type* type);

    static unsigned getBitWidth(const llvm::Type* type);
};
