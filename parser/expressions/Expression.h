#pragma once

#include "../Parser.h"

Node *parseExpression(Parser *parser, int minPrecedence = 0);
