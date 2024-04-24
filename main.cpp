#include <chrono>
#include <iostream>
#include "App.h"
#include "Builder.h"
#include "debug/timer.h"
#include "parser/Parser.h"
// Visitors
#include "preAnalysis/PreAnalysisVisitor.h"
#include "analysis/SemanticAnalysisVisitor.h"
#include "debug/PrettyPrintVisitor.h"
#include "compiler/CodegenVisitor.h"

int main(int argc, char* argv[]) {
    MEASURE("Total");

    App::Create(argc, argv);
    auto args = App::GetArgs();

    //
    // Parser
    //

    Parser* parser;
    {
        MEASURE("Parser");

        if (!args.eval.empty())
            parser = Parser::FromSource(args.eval);
        else
            parser = Parser::FromFile(args.inputs[0]);

        if (!parser)
            return 1;

        if (!parser->Parse()) return 1;

        auto totalLines = parser->lexer->CountLines();
        auto duration = timer.duration();
        auto linesPerSec = totalLines / (duration / 1000);
        std::cout << "Parsed " << totalLines
                  << " lines in " << duration
                  << "ms (" << linesPerSec << " lines/sec)"
                  << std::endl;
    }

    // return 0;

    //
    // Visitors
    //

    std::vector<std::unique_ptr<Visitor>> visitorPipeline;
    visitorPipeline.emplace_back(std::make_unique<PreAnalysisVisitor>(parser));
    visitorPipeline.emplace_back(std::make_unique<SemanticAnalysisVisitor>(parser));
    if (args.printAst)
        visitorPipeline.emplace_back(std::make_unique<PrettyPrintVisitor>(parser));
    visitorPipeline.emplace_back(std::make_unique<CodegenVisitor>(parser));

    for (const auto& visitor: visitorPipeline) {
        MEASURE(visitor->name);

        parser->ResetAstLoop();
        while (const auto& item = parser->GetNextNode()) {
            item->Accept(visitor.get());

            VisitorResult result;
            visitor->TryGetResult(result);
        }

        if (visitor->HasResults()) {
            auto results = visitor->GetResults();
            while (!results.empty()) {
                auto result = results.top();
                results.pop();

                std::cerr << "Error: " << static_cast<int>(result.errorCode)
                          << " (Success: " << result.success << ")"
                          << std::endl;
            }

            return 1;
        }
    }

    //
    // Output
    //

    if (!args.appOutput.empty()) {
        MEASURE("Build");
        if (auto buildResult = build(args); buildResult != 0)
            return buildResult;
    }

    // Print out the llvm module to file
    if (!args.llOutput.empty()) {
        MEASURE("LL Output");
        std::error_code err;
        llvm::raw_fd_ostream out(args.llOutput.string(), err);

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
    }

    return 0;
}
