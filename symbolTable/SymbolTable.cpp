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
    auto type = new TypeDefinition(
        Token{},
        StringInternTable::Intern(name)
    );
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
    auto it = functions.find(function->name);
    if (it == functions.end())
        it = functions.emplace(function->name, std::vector<Function *>()).first;

    it->second.emplace_back(function);
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

const std::vector<Function *>& SymbolTable::LookupFunction(const std::string& name) const {
    auto it = functions.find(name);
    if (it == functions.end())
        return {};

    return it->second;
}

Function* SymbolTable::LookupFunction(const std::string& name, const std::vector<TypeReference *>& args) const {
    auto it = functions.find(name);
    if (it == functions.end())
        return nullptr;

    for (const auto& function: it->second) {
        if (function->parameters.size() != args.size())
            continue;

        bool match = true;
        unsigned i = 0;
        for (const auto& param: function->parameters | std::views::values) {
            if (param->type->name != args[i++]->name) {
                match = false;
                break;
            }
        }

        if (match)
            return function;
    }

    return nullptr;
}
