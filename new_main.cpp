#include "lexer/NewLexer.h"

int main() {
    auto buffer = SourceBuffer::CreateFromFile("../../test/playground.teclang");

    auto lexer = Lexer::Lex(*buffer);
    // auto lexer = NewLexer::CreateFromFile("P:\\Javascript\\teclang-file-generator\\functions.teclang");

    return 0;
}
