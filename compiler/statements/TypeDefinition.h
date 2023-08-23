#pragma once

#include <llvm/IR/Type.h>
#include "../../ast/Visitor.h"

class TypeDefinition;

class TypeBase;

TypeBase *inferType(Visitor *v, Node *node);

llvm::Type *generateTypeDefinition(Visitor *v, TypeBase *type);

llvm::Type *generateTypeDefinition(Visitor *v, TypeDefinition *type);