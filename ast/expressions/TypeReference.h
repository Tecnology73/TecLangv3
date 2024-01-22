#pragma once

#include "../Node.h"
#include "../Visitor.h"
#include "../../misc/BitFlag.h"
#include "../StringInternTable.h"

class TypeReference : public Node {
public:
    explicit TypeReference(const Token& token, const std::string& name) : Node(token), name(name) {
    }

    static TypeReference* Infer() {
        if (!infer)
            infer = new TypeReference(Token{.type = Token::Type::Type}, StringInternTable::Intern("infer"));

        return infer;
    }

    void Accept(Visitor* visitor) override {
        visitor->Visit(this);
    }

    TypeVariant* ResolveType();

public:
    const std::string& name;
    BitFlag<TypeFlag> flags;

private:
    inline static TypeReference* infer = nullptr;

    bool resolved = false;
    TypeVariant* variant = nullptr;
};
