#pragma once

#include <cstdint>
#include "../ast/TopLevel.h"

enum class SymbolType : uint8_t {
    Unknown = 0,
    Type = 1 << 0,
    Enum = 1 << 1,
};

inline SymbolType operator|(SymbolType lhs, SymbolType rhs) {
    return static_cast<SymbolType>(
        static_cast<std::underlying_type_t<SymbolType>>(lhs) |
        static_cast<std::underlying_type_t<SymbolType>>(rhs)
    );
}

inline SymbolType operator&(SymbolType lhs, SymbolType rhs) {
    return static_cast<SymbolType>(
        static_cast<std::underlying_type_t<SymbolType>>(lhs) &
        static_cast<std::underlying_type_t<SymbolType>>(rhs)
    );
}

struct SymbolN {
    explicit SymbolN(TypeDefinition* value) : type(SymbolType::Type), value(value) {
    }

    explicit SymbolN(Enum* value) : type(SymbolType::Enum), value(value) {
    }

    SymbolType type;
    TypeBase* value;
};
