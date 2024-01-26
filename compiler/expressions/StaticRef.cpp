#include "StaticRef.h"
#include "../Compiler.h"
#include "../../symbolTable/SymbolTable.h"

void generateStaticRef(Visitor* v, StaticRef* ref) {
    if (!ref->ownerType) {
        ref->ownerType = SymbolTable::GetInstance()->GetReference(ref->name);
        if (!ref->ownerType) {
            v->ReportError(ErrorCode::STATIC_REF_UNKNOWN, {ref->name}, ref);
            return;
        }
    }

    auto key = ref->ownerType->ResolveType()->GetField(ref->next->name);
    if (!key) {
        v->ReportError(ErrorCode::ENUM_UNKNOWN_VALUE, {ref->next->name, ref->ownerType->name}, ref->next);
        return;
    }

    key->expression->Accept(v);

    if (VisitorResult result; v->TryGetResult(result))
        v->AddSuccess(result.value, ref->ownerType);
}
