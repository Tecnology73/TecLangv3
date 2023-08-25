#pragma once

#include <vector>
#include <string>
#include <format>
#include <stack>
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

    static void QueueHint(std::string msg, const std::vector<std::string> &args);

private:
    static std::stack<std::string> hintQueue;

    static void outputHint(ErrorCode code, const Parser *parser, const Node *node);
};