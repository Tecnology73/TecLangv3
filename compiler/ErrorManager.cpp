#include "ErrorManager.h"
#include "../parser/Parser.h"

void ErrorManager::Report(
    ErrorCode errorCode,
    const std::vector<std::string> &args,
    const Parser *parser,
    const Node *node
) {
    Report(errorCode, args, parser, node->token);
    outputHint(errorCode, parser, node);

    exit(static_cast<int>(errorCode));
}

void ErrorManager::Report(ErrorCode errorCode, const std::vector<std::string> &args, const struct Parser *parser, const Token &token) {
    auto it = ErrorTable.find(errorCode);
    auto errorCodeStr = std::to_string(static_cast<int>(errorCode));
    if (it == ErrorTable.end())
        throw std::runtime_error("Unknown error errorCodeStr: " + errorCodeStr);

    auto startPosition = token.position;
    auto code = parser->lexer->GetSurroundingCode(startPosition, token.position);

    Console::print("{error}[Error " + errorCodeStr + "]: " + it->second, args);
    Console::print("  {b}--> {w}{}:{}:{}", {
        parser->lexer->GetSourcePath(),
        std::to_string(startPosition.line + 1),
        std::to_string(startPosition.column)
    });
    Console::print(std::string(80, '-'), {});
    Console::print("{}", {std::get<1>(code)});
    Console::print("{error}{}{error}^~~~~~~~ {}", {
        std::string(startPosition.column + std::get<0>(code), ' '),
        Console::format(it->second, args)
    });
    Console::print("{}", {std::get<2>(code)});
    Console::print(std::string(80, '-'), {});

    // exit(static_cast<int>(errorCode));
}

void ErrorManager::outputHint(ErrorCode code, const Parser *parser, const Node *node) {
    Console::print(
        "{b}Hint{reset}: Check the types and number of parameters provided.\nAvailable constructors:\n{}",
        {"- User()\n- User(i8)"}
    );
}
