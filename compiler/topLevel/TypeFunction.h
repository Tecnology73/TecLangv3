#pragma once

#include "../../ast/topLevel/TypeBase.h"

namespace TypeFunctionCompiler {
    bool compile(Visitor* visitor, TypeBase* type);
}
