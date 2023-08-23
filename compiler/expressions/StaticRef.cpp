#include "StaticRef.h"

llvm::Value *generateStaticRef(Visitor *v, StaticRef *ref) {
    auto scope = Compiler::getScopeManager();
    if (auto type = scope.getType(ref->name)) {
        return nullptr;
    }

    if (auto anEnum = scope.getEnum(ref->name)) {
        if (!ref->next) {
            // Is this even possible??
            return nullptr;
        }

        auto valueIndex = anEnum->GetValueIndex(ref->next->name);
        if (valueIndex == -1) {
            v->ReportError(ErrorCode::ENUM_UNKNOWN_VALUE, {ref->next->name, anEnum->name}, ref->next);
            return nullptr;
        }

        return Compiler::getBuilder().getInt32(valueIndex);
    }

    v->ReportError(ErrorCode::STATIC_REF_UNKNOWN, {ref->name}, ref);
    return nullptr;
}