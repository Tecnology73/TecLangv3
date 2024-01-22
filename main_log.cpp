#include <filesystem>
#include <iostream>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/MC/TargetRegistry.h>
#include <lld/Common/Driver.h>
#include <llvm/Support/TargetSelect.h>

int printModule(const llvm::Module& module) {
    std::error_code err;
    llvm::raw_fd_ostream out("./build/out.ll", err);

    if (!err) {
        module.print(out, nullptr);
        out.flush();
        out.close();
        if (out.has_error()) {
            std::cerr << "Could not write to file: " << err.message() << std::endl;
            return 1;
        }
    } else {
        std::cerr << "Could not open file: " << err.message() << std::endl;
        return 2;
    }

    return 0;
}

int build(llvm::Module& module) {
    std::string errStr;
    llvm::TargetOptions opt;
    auto targetTriple = module.getTargetTriple();

    const llvm::Target* TheTarget = llvm::TargetRegistry::lookupTarget(targetTriple, errStr);
    std::unique_ptr<llvm::TargetMachine> targetMachine(
        TheTarget->createTargetMachine(
            targetTriple,
            "generic",
            "",
            opt,
            std::nullopt,
            std::nullopt,
            llvm::CodeGenOpt::Default
        )
    );

    // Object file
    auto objPath = std::filesystem::weakly_canonical("./build/tmp.o").string();

    llvm::SmallString<128> objFileName(objPath);
    std::error_code ec;
    llvm::raw_fd_ostream dest(objFileName, ec, llvm::sys::fs::OpenFlags::OF_None);
    if (ec) {
        std::cerr << "Could not open file: " << ec.message() << std::endl;
        return 1;
    }

    llvm::legacy::PassManager pass;
    if (targetMachine->addPassesToEmitFile(pass, dest, nullptr, llvm::CGFT_ObjectFile)) {
        std::cerr << "TargetMachine can't emit a file of this type" << std::endl;
        return 1;
    }

    pass.run(module);
    dest.flush();

    //
    // Linking
    //
    auto outArg = std::format("-out:{}", std::filesystem::weakly_canonical("./build/app.exe").string());
    std::vector<const char *> lldArgs;
    lldArgs.push_back("-subsystem:console");
    lldArgs.push_back("-entry:main");
    // lldArgs.push_back("-defaultlib:ucrt");
    // lldArgs.push_back("-defaultlib:legacy_stdio_definitions");
    // Library for GetStdHandle and WriteConsoleA
    lldArgs.push_back("-defaultlib:kernel32");
    lldArgs.push_back(objPath.c_str());
    lldArgs.push_back(outArg.c_str());

    bool success = lld::coff::link(lldArgs, llvm::outs(), llvm::errs(), false, false);
    if (!success) return 1;

    // Cleanup
    std::filesystem::remove(objPath);
    return 0;
}

void declFunctions(llvm::Module& module, llvm::IRBuilder<>& builder) {
    // open
    /*llvm::Function::Create(
        llvm::FunctionType::get(
            builder.getInt32Ty(),
            {
                builder.getInt8PtrTy(),
                builder.getInt32Ty(),
                builder.getInt32Ty()
            },
            false
        ),
        llvm::Function::ExternalLinkage,
        "open",
        module
    );*/

    // write
    /*llvm::Function::Create(
        llvm::FunctionType::get(
            builder.getInt32Ty(),
            {
                builder.getInt32Ty(),
                builder.getInt8PtrTy(),
                builder.getInt32Ty()
            },
            false
        ),
        llvm::Function::ExternalLinkage,
        "write",
        module
    );*/

    // GetStdHandle
    llvm::Function::Create(
        llvm::FunctionType::get(
            builder.getInt32Ty(),
            {
                builder.getInt32Ty()
            },
            false
        ),
        llvm::Function::ExternalLinkage,
        "GetStdHandle",
        module
    );

    // WriteConsoleA
    llvm::Function::Create(
        llvm::FunctionType::get(
            builder.getInt32Ty(),
            {
                builder.getInt32Ty(),
                builder.getInt8PtrTy(),
                builder.getInt32Ty(),
                builder.getInt32Ty(),
                builder.getInt32Ty()
            },
            false
        ),
        llvm::Function::ExternalLinkage,
        "WriteConsoleA",
        module
    );
}

int main() {
    llvm::InitializeAllTargets();
    llvm::InitializeAllTargetMCs();
    llvm::InitializeAllAsmParsers();
    llvm::InitializeAllAsmPrinters();

    auto context = llvm::LLVMContext();
    auto module = llvm::Module("MyModule", context);
    auto builder = llvm::IRBuilder<>(context);

    module.setDataLayout("e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128");
    module.setTargetTriple("x86_64-pc-windows-msvc");

    declFunctions(module, builder);

    // main func
    auto mainFunc = llvm::Function::Create(
        llvm::FunctionType::get(builder.getInt32Ty(), {}, false),
        llvm::Function::ExternalLinkage,
        "main",
        module
    );
    auto mainBlock = llvm::BasicBlock::Create(context, "entry", mainFunc);
    builder.SetInsertPoint(mainBlock);

    /*auto fd = builder.CreateCall(
        module.getFunction("open"),
        {
            builder.CreateGlobalStringPtr("/dev/stdout"),
            builder.getInt32(0),
            builder.getInt32(0)
        }
    );

    builder.CreateCall(
        module.getFunction("write"),
        {
            fd,
            builder.CreateGlobalStringPtr("Hello, world!\n"),
            builder.getInt32(14)
        }
    );*/

    auto fd = builder.CreateCall(
        module.getFunction("GetStdHandle"),
        {
            builder.getInt32(-11)
        }
    );

    builder.CreateCall(
        module.getFunction("WriteConsoleA"),
        {
            fd,
            builder.CreateGlobalStringPtr("Hello, world!\n"),
            builder.getInt32(14),
            builder.getInt32(0),
            builder.getInt32(0)
        }
    );

    builder.CreateRet(builder.getInt32(0));

    // Verify the module and generate IR file
    llvm::verifyFunction(*mainFunc);

    // return printModule(module);
    return build(module);
}
