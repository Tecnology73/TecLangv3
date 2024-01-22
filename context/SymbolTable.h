#pragma once

#include <optional>
#include <string>
#include <unordered_map>
#include <variant>

class Function;
class Enum;
class TypeDefinition;

enum class SymbolType : uint8_t {
    Unknown = 0,
    Type = 1 << 0,
    Enum = 1 << 1,
    Function = 1 << 2,
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

    explicit SymbolN(Function* value) : type(SymbolType::Function), value(value) {
    }

    SymbolType type;
    std::variant<TypeDefinition *, Enum *, Function *> value;
};

class SymbolTable {
public:
    static SymbolTable* GetInstance();

    static SymbolTable* GetInstance(const std::string& package);

    bool Add(TypeDefinition* type);

    bool Add(Enum* type);

    bool Add(Function* function);

    bool Has(const std::string_view name) const;

    bool Has(const std::string& name) const;

    bool Has(const std::string& name, const SymbolType allowedTypes) const;

    std::optional<SymbolN> Get(const std::string& name) const;

    std::optional<SymbolN> Get(const std::string& name, const SymbolType allowedTypes) const;

private:
    explicit SymbolTable();

    explicit SymbolTable(const std::string& package);

    void addBuiltinType(const std::string& name);

private:
    static SymbolTable* instance;

    std::string const package = "";

    std::unordered_map<std::string, SymbolN> symbols = {};
    std::unordered_map<std::string, SymbolTable *> packages = {};
};
