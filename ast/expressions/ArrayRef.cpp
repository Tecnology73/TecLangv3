#include "ArrayRef.h"
#include "../../compiler/Compiler.h"

TypeBase *ArrayRef::getFinalType() {
    // A ChainableNode cannot start with an ArrayRef - It's invalid syntax.
    return nullptr;
}

TypeBase *ArrayRef::getFinalType(const TypeBase *parentType) {
    return nullptr;
}
