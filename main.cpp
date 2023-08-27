#include <chrono>
#include <iostream>
#include "Console.h"
#include "parser/Parser.h"
#include "debug/PrettyPrintVisitor.h"
#include "analysis/SemanticAnalysisVisitor.h"
#include "compiler/CodegenVisitor.h"
// Related to linking
// #include <llvm/Support/FileSystem.h>
// #include <llvm/IR/LegacyPassManager.h>
// #include <llvm/Target/TargetMachine.h>
// #include <llvm/MC/TargetRegistry.h>
// #include <llvm/TargetParser/Host.h>
// #include <lld/Common/Driver.h>

// Function that records the current time
// and returns another statement to print
// the time difference.
// Credit: iDevelopThings
auto measure = [](const std::string &name) {
    auto start = std::chrono::high_resolution_clock::now();

    return [start, name]() {
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

        // printDebug("%s: %.3f ms", name.c_str(), duration / 1000.0);
        Console::print("{y}{}: {}ms", {name, std::to_string(duration / 1000.0)});
    };
};

int main() {
    auto compileTime = measure("Compile");

    //
    // Parser
    //

    auto parserTime = measure("Parser");
    // auto parser = Parser::FromSource("func main() int { return 0; }");
    auto parser = Parser::FromFile("../../test/playground.teclang");
    // auto parser = Parser::FromFile("P:\\Javascript\\teclang-file-generator\\functions.teclang");
    if (!parser->Parse()) return 1;

    parserTime();

    //
    // Visitors
    //

    std::vector<std::unique_ptr<Visitor>> visitorPipeline;
    // visitorPipeline.emplace_back(std::make_unique<SemanticAnalysisVisitor>(parser));
    // visitorPipeline.emplace_back(std::make_unique<PrettyPrintVisitor>(parser));
    visitorPipeline.emplace_back(std::make_unique<CodegenVisitor>(parser));

    for (const auto &visitor: visitorPipeline) {
        auto visitorTime = measure(visitor->name);

        for (const auto &item: parser->ast)
            item->Accept(visitor.get());

        visitorTime();
    }

    //
    // Output
    //

    // This is the final step of the compilation process.
    // It will output the llvm module to a file and then
    // link it with the lld linker.
    /*auto outputTime = measure("Output");

    std::string errStr;
    llvm::TargetOptions opt;
    // auto targetTriple = llvm::sys::getDefaultTargetTriple();
    auto targetTriple = Compiler::getModule().getTargetTriple();

    const llvm::Target *TheTarget = llvm::TargetRegistry::lookupTarget(targetTriple, errStr);
    std::unique_ptr<llvm::TargetMachine> targetMachine(TheTarget->createTargetMachine(
        targetTriple,
        "generic",
        "",
        opt,
        std::nullopt,
        std::nullopt,
        llvm::CodeGenOpt::Default
    ));

    llvm::SmallString<128> objFileName("tmp.o");
    std::error_code ec;
    llvm::raw_fd_ostream dest(objFileName, ec, llvm::sys::fs::OpenFlags::OF_None);
    if (ec) {
        std::cerr << "Could not open file: " << ec.message() << std::endl;
        return 1;
    }

    llvm::legacy::PassManager pass;
    auto fileType = llvm::CGFT_ObjectFile;
    if (targetMachine->addPassesToEmitFile(pass, dest, nullptr, fileType)) {
        std::cerr << "TargetMachine can't emit a file of this type" << std::endl;
        return 1;
    }

    pass.run(Compiler::getModule());
    dest.flush();

    // Dump the module
    // Compiler::getModule().dump();

    //
    // Linking
    //
    std::vector<const char *> args;
    args.push_back("-subsystem:console");
    args.push_back("-entry:main");
    args.push_back("tmp.o");
    args.push_back("-out:app.exe");

    bool success = lld::coff::link(args, llvm::outs(), llvm::errs(), false, false);
    if (!success) return 1;

    outputTime();*/

    // For debug purposes, dump the module to a file.
    auto outputTime = measure("Output");

    // Print out the llvm module to file
    std::error_code err;
    llvm::raw_fd_ostream out("../../out/out.ll", err);

    if (!err) {
        Compiler::getModule().print(out, nullptr);
        out.flush();
        out.close();
        if (out.has_error()) {
            std::cerr << "Could not write to file: " << err.message() << std::endl;
            return 1;
        }
    } else {
        std::cerr << "Could not open file: " << err.message() << std::endl;
        return 1;
    }

    outputTime();

    compileTime();
    return 0;
}
