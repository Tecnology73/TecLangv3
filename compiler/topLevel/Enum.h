#pragma once

#include <llvm/IR/Value.h>
#include "../../ast/TopLevel.h"
#include "../Compiler.h"

llvm::Value *generateEnum(Visitor *v, Enum *anEnum);
