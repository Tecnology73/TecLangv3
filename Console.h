#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <stdarg.h>
#include "lexer/Token.h"
#include "App.h"

struct RGB {
    int r;
    int g;
    int b;

    std::string ToString() const {
        return std::format("{};{};{}", r, g, b);
    }
};

struct Color {
    constexpr static RGB Red = {244, 67, 54};
    constexpr static RGB Green = {76, 175, 80};
    constexpr static RGB Blue = {33, 150, 243};
    constexpr static RGB Orange = {255, 152, 0};
    constexpr static RGB Gray = {158, 158, 158};
    constexpr static RGB White = {255, 255, 255};
    constexpr static RGB Yellow = {255, 235, 59};

    constexpr static RGB Debug = Gray;
    constexpr static RGB Error = Red;
    constexpr static RGB Warning = Orange;
    constexpr static RGB Info = Blue;
};

template<typename... Args>
void print(const RGB& color, const std::string& format, Args... args) {
    std::cout << "\033[38;2;" << color.ToString() << "m";
    printf(format.c_str(), args...);
    std::cout << "\033[0m" << std::endl;
}

template<typename... Args>
void printDebug(const std::string& format, Args... args) {
    if (!App::IsQuiet())
        print(Color::Debug, format, args...);
}

template<typename... Args>
void printError(const std::string& format, Args... args) {
    print(Color::Error, format, args...);
}

template<typename... Args>
void printWarning(const std::string& format, Args... args) {
    print(Color::Warning, format, args...);
}

template<typename... Args>
void printInfo(const std::string& format, Args... args) {
    if (!App::IsQuiet())
        printf(format.c_str(), args...);
}

class Console {
public:
    static void print(const std::string& message, const std::vector<std::string>& args);

    static void printRaw(const std::string& message);

    static std::string format(const std::string& str, const std::vector<std::string>& args);

    static std::string OpToString(Token::Type op);

private:
    static std::string replaceTag(const std::string& tag);
};
