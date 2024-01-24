#pragma once

#include "../../ast/topLevel/Function.h"

bool parseFunctionParameter(Parser* parser, Function* function, const bool isNameOptional = false);
