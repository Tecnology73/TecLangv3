#include "TypeBase.h"
#include "../../compiler/Compiler.h"

llvm::Type *TypeBase::getLlvmType() {
    if (!llvmType) {
        if (name == "bool")
            llvmType = llvm::Type::getInt1Ty(Compiler::getContext());
        else if (name == "i8")
            llvmType = llvm::Type::getInt8Ty(Compiler::getContext());
        else if (name == "i8*")
            llvmType = llvm::Type::getInt8PtrTy(Compiler::getContext());
        else if (name == "i16")
            llvmType = llvm::Type::getInt16Ty(Compiler::getContext());
        else if (name == "i32" || name == "int")
            llvmType = llvm::Type::getInt32Ty(Compiler::getContext());
        else if (name == "i64")
            llvmType = llvm::Type::getInt64Ty(Compiler::getContext());
        else if (name == "double")
            llvmType = llvm::Type::getDoubleTy(Compiler::getContext());
        else if (name == "void")
            llvmType = llvm::Type::getVoidTy(Compiler::getContext());
        else
            isValueType = false;

        // Compiler::getScopeManager().addCompiledType(this);
    }

    if (!isValueType)
        return llvmType->getPointerTo();

    return llvmType;
}
