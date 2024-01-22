#pragma once

#include <llvm/Support/FileSystem.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/MC/TargetRegistry.h>
#include <lld/Common/Driver.h>
#include "compiler/Compiler.h"
#include "App.h"
#include "debug/timer.h"

int build(const Arguments& args) {
    std::string errStr;
    llvm::TargetOptions opt;
    auto targetTriple = Compiler::getModule().getTargetTriple();

    const llvm::Target* TheTarget = llvm::TargetRegistry::lookupTarget(targetTriple, errStr);
    std::unique_ptr<llvm::TargetMachine> targetMachine(
        TheTarget->createTargetMachine(
            targetTriple,
            "generic",
            "",
            opt,
            std::nullopt,
            std::nullopt,
            args.optLevel
        )
    );

    // Object file
    auto objPath = weakly_canonical(args.appOutput.parent_path() / "./tmp.o").string();

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

    pass.run(Compiler::getModule());
    dest.flush();

    //
    // Linking
    //
    auto outArg = std::format("-out:{}", args.appOutput.string());
    std::vector<const char *> lldArgs;
    lldArgs.push_back("-subsystem:console");
    lldArgs.push_back("-entry:main");
    // lldArgs.push_back("-help");
    // Windows libraries
    lldArgs.push_back("-defaultlib:ucrt");
    lldArgs.push_back("-defaultlib:legacy_stdio_definitions");
    lldArgs.push_back("-defaultlib:msvcrt"); // Required for doubles...
    //
    lldArgs.push_back(objPath.c_str());
    lldArgs.push_back(outArg.c_str());

    bool success = lld::coff::link(lldArgs, llvm::outs(), llvm::errs(), false, false);
    if (!success) return 1;

    // Cleanup
    std::filesystem::remove(objPath);
    return 0;
}
