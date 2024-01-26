#pragma once

#include <unordered_map>
#include <string>
#include <stack>
#include "../ast/Node.h"
#include "../ast/expressions/TypeReference.h"

class AnalysisInfo {
public:
    explicit AnalysisInfo(Node* node) : node(node) {
    }

    void AddReturnType(Node* node, const TypeReference* type = nullptr) {
        if (!type) {
            unknownReturnTypes.push(node);
            return;
        }

        auto it = possibleReturnTypes.find(type->name);
        if (it == possibleReturnTypes.end())
            it = possibleReturnTypes.emplace(type->name, std::vector<Node *>{}).first;

        it->second.push_back(node);
    }

public:
    Node* const node;

    std::unordered_map<std::string, std::vector<Node *>> possibleReturnTypes;
    std::stack<Node *> unknownReturnTypes;
};
