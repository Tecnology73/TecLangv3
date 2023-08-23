#pragma once

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include "./scope/ScopeManager.h"

class Compiler {
public:
    Compiler() :
        module("MyModule", context),
        builder(context),
        scopeManager(context) {}

    static Compiler &getInstance() {
        static Compiler instance;
        return instance;
    }

    static llvm::LLVMContext &getContext() { return getInstance().context; }

    static llvm::Module &getModule() { return getInstance().module; }

    static llvm::IRBuilder<> &getBuilder() { return getInstance().builder; }

    static ScopeManager &getScopeManager() { return getInstance().scopeManager; }

private:
    llvm::LLVMContext context;
    llvm::Module module;
    llvm::IRBuilder<> builder;
    ScopeManager scopeManager;
};