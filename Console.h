#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <stdarg.h>

struct RGB {
    int r;
    int g;
    int b;

    std::string ToString() const {
        return std::to_string(r) + ";" + std::to_string(g) + ";" + std::to_string(b);
    }
};

struct Color {
    constexpr static const RGB Red = {244, 67, 54};
    constexpr static const RGB Green = {76, 175, 80};
    constexpr static const RGB Blue = {33, 150, 243};
    constexpr static const RGB Orange = {255, 152, 0};
    constexpr static const RGB Gray = {158, 158, 158};
    constexpr static const RGB White = {255, 255, 255};

    constexpr static const RGB Debug = Gray;
    constexpr static const RGB Error = Red;
    constexpr static const RGB Warning = Orange;
    constexpr static const RGB Info = Blue;
};

template<typename... Args>
void print(RGB color, const std::string &format, Args... args) {
    std::cout << "\033[38;2;" << color.ToString() << "m";
    printf(format.c_str(), args...);
    std::cout << "\033[0m" << std::endl;
}

template<typename... Args>
void printDebug(const std::string &format, Args... args) {
    print(Color::Debug, format, args...);
}

template<typename... Args>
void printError(const std::string &format, Args... args) {
    print(Color::Error, format, args...);
}

template<typename... Args>
void printWarning(const std::string &format, Args... args) {
    print(Color::Warning, format, args...);
}

template<typename... Args>
void printInfo(const std::string &format, Args... args) {
    // print(Color::Info, format, args...);
    printf(format.c_str(), args...);
}

class Console {
public:
    static void print(const std::string &message, const std::vector<std::string> &args);

    static std::string format(const std::string &str, const std::vector<std::string> &args);

private:
    static std::string replaceTag(const std::string &tag);
};