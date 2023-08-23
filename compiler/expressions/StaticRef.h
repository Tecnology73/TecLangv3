#pragma once

#include <llvm/IR/Value.h>
#include "../../ast/Expressions.h"
#include "../Compiler.h"

llvm::Value *generateStaticRef(Visitor *v, StaticRef *ref);