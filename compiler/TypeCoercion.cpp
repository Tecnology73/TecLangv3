#include "TypeCoercion.h"

std::pair<llvm::Value *, llvm::Value *> TypeCoercion::coerce(llvm::Value* valueA, llvm::Value* valueB) {
    auto priorityA = getTypePriority(valueA->getType());
    auto priorityB = getTypePriority(valueB->getType());

    if (priorityA < priorityB) // Upcast A -> B
        valueA = coerce(valueA, valueB->getType());
    else if (priorityA > priorityB) // Upcast B -> A
        valueB = coerce(valueB, valueA->getType());
    /*else
        coerce(valueA, valueB);*/

    return {valueA, valueB};
}

llvm::Value* TypeCoercion::coerce(llvm::Value* value, llvm::Type* toType, bool forcePointerCast) {
    if (!forcePointerCast && value->getType()->isPointerTy())
        return value; // TODO: Implement pointer coercion?

    if (toType->isIntegerTy())
        return coerceToInt(value, toType);

    if (toType->isDoubleTy())
        return coerceToDouble(value, toType);

    // return nullptr;
    // Will this break anything if we don't return nullptr?
    // Technically, if nullptr is returned, shit would break anyways...
    return value;
}

TypeReference* TypeCoercion::getCommonType(TypeReference* typeA, TypeReference* typeB) {
    auto priorityA = getTypePriority(typeA->ResolveType());
    auto priorityB = getTypePriority(typeB->ResolveType());

    if (priorityA == priorityB) // Same type
        return typeA;

    if (priorityA == 0 || priorityB == 0) // Object?
        return nullptr;

    if (priorityA > priorityB) // Upcast A -> B
        return typeA;

    if (priorityA < priorityB) // Upcast B -> A
        return typeB;

    return nullptr; // Differing/Incompatible types
}

TypeBase* TypeCoercion::getCommonType(TypeBase* typeA, TypeBase* typeB) {
    auto priorityA = getTypePriority(typeA);
    auto priorityB = getTypePriority(typeB);

    if (priorityA == priorityB) // Same type
        return typeA;

    if (priorityA == 0 || priorityB == 0) // Object?
        return nullptr;

    if (priorityA > priorityB) // Upcast A -> B
        return typeA;

    if (priorityA < priorityB) // Upcast B -> A
        return typeB;

    return nullptr; // Differing/Incompatible types
}

llvm::Type* TypeCoercion::getCommonType(llvm::Type* typeA, llvm::Type* typeB) {
    auto priorityA = getTypePriority(typeA);
    auto priorityB = getTypePriority(typeB);

    if (priorityA > priorityB)
        return typeA;

    if (priorityA < priorityB)
        return typeB;

    return typeA;
}

bool TypeCoercion::isTypeCompatible(TypeReference* type, TypeReference* otherType) {
    return isTypeCompatible(type->ResolveType(), otherType->ResolveType());
}

bool TypeCoercion::isTypeCompatible(const TypeBase* type, const TypeBase* otherType) {
    if (!type->isValueType) {
        if (type->name == otherType->name) // Same type
            return true;

        // Everything (that's not a value type) can be casted to a raw pointer.
        if (otherType->name == "ptr") {
            return true;
        }

        // TODO: Check if type is a subtype of otherType
        return false;
    }

    if (!otherType->isValueType)
        return false;

    return getTypePriority(type) <= getTypePriority(otherType);
}

bool TypeCoercion::canCoerceTo(const TypeBase* fromType, const TypeBase* toType) {
    if (!toType->isValueType || !fromType->isValueType)
        return false;

    auto priorityA = getTypePriority(fromType);
    auto priorityB = getTypePriority(toType);

    if (priorityA == priorityB) // Same type
        return true;

    if (priorityB > priorityA) // B can "fit inside" A (i8 -> i32)
        return true;

    return false; // Differing/Incompatible types
}

int TypeCoercion::getTypePriority(const llvm::Type* type) {
    if (type->isIntegerTy()) {
        auto width = type->getIntegerBitWidth();
        if (width == 1) return static_cast<int>(Priority::Boolean);
        if (width == 8) return static_cast<int>(Priority::Int8);
        if (width == 16) return static_cast<int>(Priority::Int16);
        if (width == 32) return static_cast<int>(Priority::Int32);
        if (width == 64) return static_cast<int>(Priority::Int64);
    }
    if (type->isDoubleTy()) return static_cast<int>(Priority::Double);

    return static_cast<int>(Priority::None);
}

int TypeCoercion::getTypePriority(const TypeBase* type) {
    if (type->name == "bool") return static_cast<int>(Priority::Boolean);
    if (type->name == "i8") return static_cast<int>(Priority::Int8);
    if (type->name == "u8") return static_cast<int>(Priority::UInt8);
    if (type->name == "i16") return static_cast<int>(Priority::Int16);
    if (type->name == "u16") return static_cast<int>(Priority::UInt16);
    if (type->name == "i32") return static_cast<int>(Priority::Int32);
    if (type->name == "u32") return static_cast<int>(Priority::UInt32);
    if (type->name == "i64") return static_cast<int>(Priority::Int64);
    if (type->name == "u64") return static_cast<int>(Priority::UInt64);
    if (type->name == "double") return static_cast<int>(Priority::Double);

    return static_cast<int>(Priority::None);
}

llvm::Value* TypeCoercion::coerceToInt(llvm::Value* value, llvm::Type* type) {
    if (value->getType()->isDoubleTy()) {
        if (getBitWidth(type) < 64)
            return nullptr;

        return Compiler::getBuilder().CreateFPToSI(value, type);
    }

    unsigned valueBitWidth = getBitWidth(value->getType());
    unsigned targetBitWidth = getBitWidth(type);

    if (valueBitWidth < targetBitWidth)
        return Compiler::getBuilder().CreateZExt(value, type);
    /*else if (valueBitWidth > targetBitWidth)
        // TODO: Make sure that down-casts are actually explicit.
        // return Compiler::getBuilder().CreateTrunc(expression, type);
        return nullptr;*/

    return value;
}

llvm::Value* TypeCoercion::coerceToDouble(llvm::Value* value, llvm::Type* type) {
    if (value->getType()->isDoubleTy())
        return value;

    if (value->getType()->isIntegerTy())
        return Compiler::getBuilder().CreateSIToFP(value, type);

    return nullptr;
}

unsigned TypeCoercion::getBitWidth(const llvm::Type* type) {
    if (type->isIntegerTy())
        return type->getIntegerBitWidth();

    return 0;
}
