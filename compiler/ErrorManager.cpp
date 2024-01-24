#include "ErrorManager.h"
#include "../parser/Parser.h"
#include "../Console.h"

std::stack<std::string> ErrorManager::hintQueue;

void ErrorManager::Report(
    const ErrorCode errorCode,
    const std::vector<std::string>& args,
    const Parser* parser,
    const Node* node
) {
    Report(errorCode, args, parser, node->token);
}

void ErrorManager::Report(
    ErrorCode errorCode,
    const std::vector<std::string>& args,
    const Parser* parser,
    const Token& token
) {
    auto it = ErrorTable.find(errorCode);
    auto errorCodeStr = std::to_string(static_cast<int>(errorCode));
    if (it == ErrorTable.end())
        throw std::runtime_error("Unknown error errorCodeStr: " + errorCodeStr);

    // Error message.
    Console::print("{error}[Error " + errorCodeStr + "]{reset}: " + it->second, args);
    // Source location.
    Console::print(
        "  {b}--> {w}{}:{}:{}",
        {
            parser->lexer->GetSourcePath(),
            std::to_string(token.position.line + 1),
            std::to_string(token.position.column)
        }
    );
    //
    Console::printRaw(std::string(80, '-'));

    // Source code.
    long topHalfIndex;
    auto code = parser->lexer->GetSurroundingCode(token.position, token.position, topHalfIndex);

    // Calculate the number of spaces needed so that the line number is aligned.
    int spaces = std::to_string(token.position.line + code.size()).length();

    for (int i = 0; i < code.size(); i++) {
        // I'm too stupid to understand why I need to do `i - topHalfIndex + 1` here.
        auto line = std::to_string(token.position.line + i - topHalfIndex + 1);

        Console::print(
            "{b}{}{} | {reset}{}",
            {
                std::string(spaces - line.length(), ' '),
                line,
                code[i]
            }
        );

        // Inline error message.
        if (i == topHalfIndex) {
            Console::print(
                "{b}{} | {error}{}{} {}",
                {
                    std::string(spaces, ' '),
                    std::string(std::max<int>(0, token.position.column + spaces - line.length()), ' '),
                    std::string(token.position.length, '^'),
                    Console::format(it->second, args)
                }
            );
        }
    }

    Console::printRaw(std::string(80, '-') + "\n");
    outputHint();

    // exit(static_cast<int>(errorCode));
}

void ErrorManager::outputHint() {
    if (hintQueue.empty()) return;

    Console::printRaw(std::string(80, '-'));
    Console::printRaw(hintQueue.top());
    hintQueue.pop();
}

void ErrorManager::QueueHint(const std::string& msg, const std::vector<std::string>& args) {
    hintQueue.emplace(Console::format("{b}Hint{reset}: " + msg, args));
}
