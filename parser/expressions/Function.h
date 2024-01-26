#pragma once

#include "../../ast/topLevel/Function.h"

bool parseParameters(Parser* parser, Function* function, const bool isParamNameOptional = false);

Function* parseFunction(Parser* parser);
