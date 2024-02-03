#pragma once

#include "../../ast/expressions/TypeReference.h"

namespace TypeCompiler {
    /// <summary>
    /// Gets the default value for the given type.
    /// The reference (<paramref name="type"/>) is passed in because that is what holds the Optional flag.
    /// </summary>
    llvm::Value* getDefaultValue(TypeReference* type);

    llvm::Type* compile(TypeReference* type);
}
