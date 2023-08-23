#pragma once

#include "../Parser.h"
#include "../../ast/Expressions.h"

ChainableNode *parseChainable(Parser *parser);

VariableReference *parseVariableReference(Parser *parser);