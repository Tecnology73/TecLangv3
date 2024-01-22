#pragma once

#include <stack>
#include <string>
#include <vector>
#include "Token.h"

class Lexer {
public:
    explicit Lexer(const std::string& source);

    Lexer(const std::string& source, const std::string& sourcePath) : Lexer(source) {
        this->sourcePath = sourcePath;
    }

    const std::string& GetSourcePath() const {
        return sourcePath;
    }

    Token GetNextToken(bool useStack = true);

    Token PeekToken();

    bool IsAtEnd() const;

    size_t CountLines() const;

    std::vector<std::string> GetSurroundingCode(
        const Position& startPos,
        const Position& endPos,
        long& topHalfIndex
    );

    static Lexer* FromSource(const std::string& source);

    static Lexer* FromFile(const std::string& path);

private:
    inline char consumeChar();

    inline char peekChar(int offset = 1) const;

    void consumeUntilLine(long line);

    Token parseIdentifier();

    Token parseNumber();

    Token parseString();

    Token parsePunctuation();

    void parseLineComment();

    void parseBlockComment();

    inline Token makeToken(Token::Type type, std::string_view value, const Position& pos) const;

    inline Token makeToken(Token::Type type, double value, const Position& pos) const;

    inline Token makeToken(Token::Type type, int value, const Position& pos) const;

public:
    Position position = {0, 0, 0};

private:
    std::string sourcePath = "inline_source_file";
    std::string source;
    long sourceLength;

    // Buffer for tokens parsed during `consumeUntilLine`.
    // This will be returned by `GetNextToken` until it's empty.
    std::stack<Token> tokens;

    std::vector<std::pair<long, long>> lines;
    long currentLineStartIndex = 0;
};
