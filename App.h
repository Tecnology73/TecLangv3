#pragma once

#include <filesystem>
#include <vector>
#include <string>
#include <llvm/Support/Codegen.h>

struct Arguments {
    std::filesystem::path currentDir;
    std::vector<std::string> inputs;
    std::string eval;
    std::filesystem::path llOutput;
    std::filesystem::path appOutput;
    llvm::CodeGenOpt::Level optLevel;
    bool printAst = false;
    bool quiet = false;
};

class App {
public:
    explicit App(Arguments args) {
        instance = this;
        this->args = args;
    }

    static App* Create(int argc, char* argv[]);

    [[nodiscard]] static const Arguments& GetArgs() {
        return instance->args;
    }

    [[nodiscard]] static bool IsQuiet() {
        return instance->args.quiet;
    }

private:
    static App* instance;

    Arguments args;
};
