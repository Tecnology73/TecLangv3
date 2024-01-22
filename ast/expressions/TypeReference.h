#pragma once

#include "../Node.h"
#include "../Visitor.h"
#include "../../misc/BitFlag.h"

class TypeReference : public Node {
public:
    explicit TypeReference(const Token& token) : Node(token), name(token.value) {
    }

    explicit TypeReference(TypeBase* type) : Node(type->token), name(type->name) {
        resolved = true;
        variant = type->createVariant();
    }

    static TypeReference* Infer() {
        if (!infer)
            infer = new TypeReference(Token{.type = Token::Type::Type, .value = "infer"});

        return infer;
    }

    void Accept(Visitor* visitor) override {
        visitor->Visit(this);
    }

    TypeVariant* ResolveType();

public:
    std::string const name;
    BitFlag<TypeFlag> flags;

private:
    inline static TypeReference* infer = nullptr;

    bool resolved = false;
    TypeVariant* variant = nullptr;
};
