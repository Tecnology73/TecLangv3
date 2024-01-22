#include "SymbolTable.h"
#include "../ast/TopLevel.h"

SymbolTable* SymbolTable::instance = nullptr;

SymbolTable::SymbolTable() {
    addBuiltinType("bool");
    addBuiltinType("i8");
    addBuiltinType("i8*");
    addBuiltinType("i16");
    addBuiltinType("i32");
    addBuiltinType("int");
    addBuiltinType("i64");
    addBuiltinType("double");
    addBuiltinType("void");
}

SymbolTable::SymbolTable(const std::string& package) : package(package) {
}

void SymbolTable::addBuiltinType(const std::string& name) {
    auto type = new TypeDefinition(Token{}, name);
    type->isDeclared = true;
    type->isValueType = true;

    Add(type);
}

SymbolTable* SymbolTable::GetInstance() {
    if (!instance)
        instance = new SymbolTable();

    return instance;
}

SymbolTable* SymbolTable::GetInstance(const std::string& package) {
    auto instance = GetInstance();
    auto it = instance->packages.find(package);
    if (it == instance->packages.end())
        it = instance->packages.emplace(package, new SymbolTable(package)).first;

    return it->second;
}

bool SymbolTable::Add(TypeDefinition* type) {
    if (Has(type->name))
        return false;

    symbols.emplace(type->name, SymbolN(type));
    return true;
}

bool SymbolTable::Add(Enum* type) {
    if (Has(type->name))
        return false;

    symbols.emplace(type->name, SymbolN(type));
    return true;
}

bool SymbolTable::Add(Function* function) {
    if (Has(function->name))
        return false;

    symbols.emplace(function->name, SymbolN(function));
    return true;
}

bool SymbolTable::Has(const std::string_view name) const {
    return Get(std::string(name)).has_value();
}

bool SymbolTable::Has(const std::string& name) const {
    return Get(name).has_value();
}

bool SymbolTable::Has(const std::string& name, const SymbolType allowedTypes) const {
    return Get(name, allowedTypes).has_value();
}

std::optional<SymbolN> SymbolTable::Get(const std::string& name) const {
    auto it = symbols.find(name);
    if (it == symbols.end())
        return std::nullopt;

    return it->second;
}

std::optional<SymbolN> SymbolTable::Get(const std::string& name, const SymbolType allowedTypes) const {
    auto it = symbols.find(name);
    if (it == symbols.end() || (it->second.type & allowedTypes) == SymbolType::Unknown)
        return std::nullopt;

    return it->second;
}
