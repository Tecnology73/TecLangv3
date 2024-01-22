#include "App.h"
#include <iostream>

App* App::instance = nullptr;

App* App::Create(int argc, char* argv[]) {
    if (instance) {
        std::cerr << "App already created." << std::endl;
        exit(1);
    }

    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <source file>" << std::endl;
        exit(1);
    }

    Arguments args = {
        .currentDir = std::filesystem::current_path(),
        // .llOutput = weakly_canonical(currentDir / "./out/out.ll").string(),
        // .appOutput = weakly_canonical(currentDir / "./out/app.exe").string(),
        .optLevel = llvm::CodeGenOpt::Default
    };

    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];

        if (arg == "-ll") {
            if (i + 1 >= argc) {
                std::cerr << "-ll option requires one argument." << std::endl;
                exit(1);
            }

            args.llOutput = weakly_canonical(args.currentDir / argv[++i]);
        } else if (arg == "-build") {
            if (i + 1 >= argc) {
                std::cerr << "-build option requires one argument." << std::endl;
                exit(1);
            }

            args.appOutput = weakly_canonical(args.currentDir / argv[++i]);
        } else if (arg == "-eval") {
            if (i + 1 >= argc) {
                std::cerr << "-eval option requires one argument." << std::endl;
                exit(1);
            }

            if (!args.inputs.empty()) {
                std::cerr << "Cannot use -eval with other input files." << std::endl;
                exit(1);
            }

            auto source = argv[++i];
            // Strip quotes
            if (source[0] == '"' && source[strlen(source) - 1] == '"') {
                source[strlen(source) - 1] = '\0';
                source++;
            }

            args.eval = source;
        } else if (arg == "-quiet")
            args.quiet = true;
        else if (arg == "-O0")
            args.optLevel = llvm::CodeGenOpt::None;
        else if (arg == "-O1")
            args.optLevel = llvm::CodeGenOpt::Less;
        else if (arg == "-O2")
            args.optLevel = llvm::CodeGenOpt::Default;
        else if (arg == "-O3")
            args.optLevel = llvm::CodeGenOpt::Aggressive;
        else if (arg == "-printAst")
            args.printAst = true;
        else if (arg.starts_with('-')) {
            std::cerr << "Unknown option: " << arg << std::endl;
            exit(1);
        } else {
            if (!args.eval.empty()) {
                std::cerr << "Cannot use -eval with other input files." << std::endl;
                exit(1);
            }

            args.inputs.push_back(weakly_canonical(args.currentDir / arg).string());
        }
    }

    if (args.inputs.empty() && args.eval.empty()) {
        std::cerr << "No input files." << std::endl;
        exit(1);
    }

    if (args.llOutput.empty() && args.appOutput.empty()) {
        std::cerr << "One of -build or -ll must be provided." << std::endl;
        exit(1);
    }

    return new App(args);
}
