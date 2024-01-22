#include "TypeVariant.h"
#include "../expressions/TypeReference.h"
#include "../../compiler/Compiler.h"

TypeVariant::TypeVariant(TypeBase* type) : type(type) {
    Compiler::getScopeManager().addTypeUse(this, type);
}

TypeVariant* TypeVariant::Clone() const {
    auto clone = new TypeVariant(type);
    clone->flags = flags;

    return clone;
}

TypeReference* TypeVariant::CreateReference() const {
    auto ref = new TypeReference(type->token, StringInternTable::Intern(type->name));
    ref->flags = flags;

    return ref;
}


void TypeVariant::SetType(TypeBase* type) {
    this->type = type;
    Compiler::getScopeManager().addTypeUse(this, type);
}
