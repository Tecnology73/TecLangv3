#pragma once

#include <llvm/Support/MemoryBuffer.h>
#include <string>

struct Token {
    uint32_t index;
};

struct TokenInfo {
    uint32_t kind;
    uint32_t line;
    uint32_t column;
};

class SourceBuffer {
public:
    SourceBuffer(std::string fileName, std::unique_ptr<llvm::MemoryBuffer> buffer) : fileName(std::move(fileName)), buffer(std::move(buffer)) {}

    static std::unique_ptr<SourceBuffer> CreateFromFile(const std::string &fileName) {
        auto buffer = llvm::MemoryBuffer::getFile(fileName);
        if (!buffer) return nullptr;

        return std::make_unique<SourceBuffer>(fileName, std::move(buffer.get()));
    }

    std::string const fileName;
    std::unique_ptr<llvm::MemoryBuffer> const buffer;
};

class Lexer {
public:
    explicit Lexer(SourceBuffer &buffer) : source(&buffer) {}

    static Lexer Lex(SourceBuffer &buffer) {
        Lexer lexer(buffer);

        auto sourceText = buffer.buffer->getBuffer();
        while (!sourceText.empty()) {
            switch (sourceText.front()) {
                case ' ':
                case '\t':
                    ++lexer.column;
                    sourceText = sourceText.drop_front();
                    break;
                case '\n':
                    ++lexer.line;
                    lexer.column = 0;
                    sourceText = sourceText.drop_front();
                    break;
            }
        }

        return lexer;
    }

public:
    SourceBuffer *source;
    uint32_t column;
    uint32_t line;

    std::vector<TokenInfo> tokens;
};
