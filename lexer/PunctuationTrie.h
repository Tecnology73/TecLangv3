#pragma once

#include <string>
#include <unordered_map>
#include "Token.h"

struct TrieNode {
    std::unordered_map<char, TrieNode *> children;
    Token::Type tokenType = Token::Type::Punctuation;
};

class PunctuationTrie {
public:
    PunctuationTrie(
        std::initializer_list<std::pair<std::string, Token::Type>> punctuation
    ) : root(new TrieNode()) {
        for (auto& pair: punctuation)
            insert(pair.first, pair.second);
    }

    void insert(const std::string& symbol, Token::Type tokenType) {
        TrieNode* curr = root;
        for (char c: symbol) {
            if (curr->children.count(c) == 0)
                curr->children[c] = new TrieNode();

            curr = curr->children[c];
        }

        curr->tokenType = tokenType;
    }

    Token::Type findTokenType(const std::string& symbol) const {
        TrieNode* curr = root;
        for (char c: symbol) {
            if (curr->children.count(c) == 0)
                return Token::Type::Punctuation;

            curr = curr->children[c];
        }

        return curr->tokenType;
    }

    TrieNode* getRoot() const {
        return root;
    }

private:
    TrieNode* root;
};
