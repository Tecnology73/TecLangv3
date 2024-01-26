#pragma once

#include <optional>
#include <string>
#include <unordered_map>
#include "SymbolN.h"
#include "../ast/expressions/TypeReference.h"

class TypeDefinition;

class Enum;

class Function;

class SymbolTable {
public:
    static SymbolTable* GetInstance();

    static SymbolTable* GetInstance(const std::string& package);

    bool Add(TypeDefinition* type);

    bool Add(Enum* type);

    bool Add(Function* function);

    bool Has(const std::string_view name) const;

    bool Has(const std::string& name) const;

    std::optional<SymbolN> Get(const std::string& name) const;

    template<typename T>
    T* Get(const std::string& name) const {
        static_assert(std::is_base_of_v<TypeBase, T>, "T must be derived from TypeBase");

        auto it = symbols.find(name);
        if (it == symbols.end())
            return nullptr;

        if (auto type = dynamic_cast<T *>(it->second.value))
            return type;

        return nullptr;
    }

    TypeReference* GetReference(const std::string& name) const {
        auto type = Get(name);
        if (!type.has_value())
            return nullptr;

        return type->value->CreateReference();
    }

    template<typename T>
    TypeReference* GetReference(const std::string& name) const {
        static_assert(std::is_base_of_v<TypeBase, T>, "T must be derived from TypeBase");

        auto type = Get<T>(name);
        if (!type)
            return nullptr;

        return type->CreateReference();
    }

    const std::vector<Function *>& LookupFunction(const std::string& name) const;

    Function* LookupFunction(const std::string& name, const std::vector<TypeReference *>& args) const;

private:
    explicit SymbolTable();

    explicit SymbolTable(const std::string& package);

    void addBuiltinType(const std::string& name);

private:
    static SymbolTable* instance;

    std::string const package = "";

    std::unordered_map<std::string, SymbolN> symbols = {};
    std::unordered_map<std::string_view, std::vector<Function *>> functions = {};
    std::unordered_map<std::string, SymbolTable *> packages = {};
};
