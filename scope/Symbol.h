#pragma once

#include "../ast/topLevel/TypeBase.h"

class Symbol {
public:
    Symbol(class Node* node, TypeReference* typeRef);

    ~Symbol();

    std::shared_ptr<Symbol> GetField(const std::string& name);

public:
    const Node* const node;
    TypeReference* const type;

private:
    // Maintains a list of fields and their types that can be modified as we traverse the AST.
    std::unordered_map<std::string_view, std::shared_ptr<Symbol>> fieldCache;
};
