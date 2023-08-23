#pragma once

#include <vector>
#include <string>
#include <format>
#include "ErrorTable.h"
#include "../Console.h"
#include "../ast/Node.h"

class ErrorManager {
public:
    static void Report(
        ErrorCode errorCode,
        const std::vector<std::string> &args,
        const class Parser *parser,
        const Node *node
    );

    static void Report(ErrorCode errorCode, const std::vector<std::string> &args, const class Parser *parser, const Token &token);

private:
    static void outputHint(ErrorCode code, const Parser *parser, const Node *node);
};