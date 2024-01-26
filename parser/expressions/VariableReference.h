#pragma once

#include "../../ast/expressions/VarReference.h"

ChainableNode* parseChainable(Parser* parser);

VariableReference* parseVariableReference(Parser* parser);
