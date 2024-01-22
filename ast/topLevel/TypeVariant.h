#pragma once

#include "TypeBase.h"

class TypeVariant {
public:
    explicit TypeVariant(TypeBase* type);

    TypeVariant* Clone() const;

    TypeReference* CreateReference() const;

    void SetType(TypeBase* type);

public:
    TypeBase* type;
    BitFlag<TypeFlag> flags;
};
