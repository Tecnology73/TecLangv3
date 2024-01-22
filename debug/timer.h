#pragma once

#include "../Console.h"

class Timer {
public:
    explicit Timer(std::string name) : name(std::move(name)), start(std::chrono::high_resolution_clock::now()) {
    }

    ~Timer() {
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

        if (!App::IsQuiet())
            Console::print("{y}{}: {}ms", {name, std::to_string(duration / 1000.0)});
    }

    double duration() {
        auto end = std::chrono::high_resolution_clock::now();

        return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() / 1000.0;
    }

private:
    std::string const name;
    std::chrono::time_point<std::chrono::high_resolution_clock> const start;
};

#define MEASURE(name) Timer timer(name)
