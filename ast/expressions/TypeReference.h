#pragma once

#include "../Node.h"
#include "../Visitor.h"
#include "../../misc/BitFlag.h"

class TypeReference : public Node {
public:
    explicit TypeReference(const Token& token, const std::string& name);

    void Accept(Visitor* visitor) override;

    TypeReference* Clone() const;

    TypeBase* ResolveType();

public:
    const std::string& name;
    BitFlag<TypeFlag> flags;

private:
    inline static TypeReference* infer = nullptr;

    bool resolved = false;
    TypeBase* concreteType = nullptr;
};
