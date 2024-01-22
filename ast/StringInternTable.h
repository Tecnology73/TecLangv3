#pragma once

#include <string>
#include <unordered_set>

class StringInternTable {
public:
    static const std::string& Intern(const std::string_view& str) {
        if (instance == nullptr)
            instance = new StringInternTable();

        return *instance->strings.emplace(str).first;
    }

// private:
    inline static StringInternTable* instance = nullptr;

    std::unordered_set<std::string> strings = {};
};
