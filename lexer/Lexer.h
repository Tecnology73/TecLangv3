#pragma once

#include <string>
#include <vector>
#include "Token.h"

class Lexer {
public:
    explicit Lexer(const std::string &source);

    Lexer(const std::string &source, const std::string &sourcePath) : Lexer(source) {
        this->sourcePath = sourcePath;
    }

    const std::string &GetSourcePath() const {
        return sourcePath;
    }

    Token GetNextToken();

    Token PeekToken();

    bool IsAtEnd() const;

    std::tuple<unsigned int, std::string, std::string> GetSurroundingCode(Position startPos, Position endPos);

    static Lexer *FromSource(const std::string &source);

    static Lexer *FromFile(const std::string &path);

private:
    inline char consumeChar();

    inline char peekChar(int offset = 1) const;

    Token parseIdentifier();

    Token parseNumber();

    Token parseString();

    Token parsePunctuation();

    void parseLineComment();

    void parseBlockComment();

    inline Token makeToken(Token::Type type, std::string value, Position pos) const;

    inline Token makeToken(Token::Type type, double value, Position pos) const;

    inline Token makeToken(Token::Type type, int value, Position pos) const;

public:
    Position position = {0, 0, 0};

private:
    std::string sourcePath = "inline_source_file";
    std::string source;
    std::vector<std::string> lines;
    long sourceLength;
};
