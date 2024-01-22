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

    bool Has(const std::string& name, const SymbolType allowedTypes) const;

    std::optional<SymbolN> Get(const std::string& name) const;

    std::optional<SymbolN> Get(const std::string& name, const SymbolType allowedTypes) const;

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
