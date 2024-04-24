#pragma once

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/Support/TargetSelect.h>

class Compiler {
public:
    Compiler() : module("MyModule", context),
                 builder(context) {
        llvm::InitializeAllTargets();
        llvm::InitializeAllTargetMCs();
        llvm::InitializeAllAsmParsers();
        llvm::InitializeAllAsmPrinters();

        // Setup module
        module.setDataLayout("e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128");
        module.setTargetTriple("x86_64-pc-windows-msvc");
    }

    static Compiler& getInstance() {
        static Compiler instance;
        return instance;
    }

    static llvm::LLVMContext& getContext() { return getInstance().context; }

    static llvm::Module& getModule() { return getInstance().module; }

    static llvm::IRBuilder<>& getBuilder() { return getInstance().builder; }

private:
    llvm::LLVMContext context;
    llvm::Module module;
    llvm::IRBuilder<> builder;
};
