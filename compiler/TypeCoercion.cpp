#include "TypeCoercion.h"

std::pair<llvm::Value *, llvm::Value *> TypeCoercion::coerce(llvm::Value *valueA, llvm::Value *valueB) {
    int priorityA = getTypePriority(valueA->getType());
    int priorityB = getTypePriority(valueB->getType());

    if (priorityA < priorityB)
        // Up-cast valueA to valueB's type.
        valueA = coerce(valueA, valueB->getType());
    else if (priorityB < priorityA)
        // Up-cast valueB to valueA's type.
        valueB = coerce(valueB, valueA->getType());

    return {valueA, valueB};
}

llvm::Value *TypeCoercion::coerce(llvm::Value *value, llvm::Type *toType) {
    if (value->getType()->isPointerTy())
        return value; // TODO: Implement pointer coercion?

    if (toType->isIntegerTy())
        return coerceToInt(value, toType);

    if (toType->isDoubleTy())
        return coerceToDouble(value, toType);
}

bool TypeCoercion::canCoerceTo(TypeBase *fromType, TypeBase *toType) {
    if (!toType->isValueType)
        return false;

    // Allow casting up but not down.
    if (fromType->name == "bool")
        return toType->name == "bool" ||
               toType->name == "i8" ||
               toType->name == "i16" ||
               toType->name == "i32" ||
               toType->name == "i64" ||
               toType->name == "double";
    else if (fromType->name == "i8")
        return toType->name == "i8" ||
               toType->name == "i16" ||
               toType->name == "i32" ||
               toType->name == "i64" ||
               toType->name == "double";
    else if (fromType->name == "i16")
        return toType->name == "i16" ||
               toType->name == "i32" ||
               toType->name == "i64" ||
               toType->name == "double";
    else if (fromType->name == "i32")
        return toType->name == "i32" ||
               toType->name == "i64" ||
               toType->name == "double";
    else if (fromType->name == "i64")
        return toType->name == "i64" ||
               toType->name == "double";
    else if (fromType->name == "double")
        return toType->name == "double";

    throw std::runtime_error("Unhandled value type: " + fromType->name);
}

int TypeCoercion::getTypePriority(llvm::Type *type) {
    if (type->isIntegerTy()) return static_cast<int>(Priority::Integer);
    if (type->isDoubleTy()) return static_cast<int>(Priority::Double);

    return static_cast<int>(Priority::None);
}

llvm::Value *TypeCoercion::coerceToInt(llvm::Value *value, llvm::Type *type) {
    unsigned valueBitWidth = getBitWidth(value->getType());
    unsigned targetBitWidth = getBitWidth(type);

    if (valueBitWidth < targetBitWidth)
        return Compiler::getBuilder().CreateZExt(value, type);
    else if (valueBitWidth > targetBitWidth)
        // TODO: Make sure that down-casts are actually explicit.
        // return Compiler::getBuilder().CreateTrunc(value, type);
        return nullptr;

    return value;
}

llvm::Value *TypeCoercion::coerceToDouble(llvm::Value *value, llvm::Type *type) {
    if (value->getType()->isDoubleTy())
        return value;

    if (value->getType()->isIntegerTy())
        return Compiler::getBuilder().CreateSIToFP(value, type);

    return nullptr;
}

unsigned TypeCoercion::getBitWidth(llvm::Type *type) {
    if (type->isIntegerTy())
        return type->getIntegerBitWidth();

    return 0;
}
