#include <chrono>
#include <iostream>
#include "parser/Parser.h"
#include "compiler/PrettyPrintVisitor.h"
#include "compiler/CodegenVisitor.h"
#include "Console.h"

// Function that records the current time
// and returns another statement to print
// the time difference.
// Credit: iDevelopThings
auto measure = [](const std::string &name) {
    auto start = std::chrono::high_resolution_clock::now();

    return [start, name]() {
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

        printDebug("%s: %.3f ms", name.c_str(), duration / 1000.0);
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
    if (!parser->Parse()) return 1;

    parserTime();

    //
    // Visitors
    //

    std::vector<std::unique_ptr<Visitor>> visitorPipeline;
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
