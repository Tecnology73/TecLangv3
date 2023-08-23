#include "Console.h"

void Console::print(const std::string &message, const std::vector<std::string> &args) {
    std::cout << format(message + "{reset}", args) << std::endl;
}

std::string Console::replaceTag(const std::string &tag) {
    if (tag == "r") return "\033[38;2;" + Color::Red.ToString() + "m";
    if (tag == "g") return "\033[38;2;" + Color::Green.ToString() + "m";
    if (tag == "b") return "\033[38;2;" + Color::Blue.ToString() + "m";
    if (tag == "o") return "\033[38;2;" + Color::Orange.ToString() + "m";
    if (tag == "y") return "\033[38;2;" + Color::Gray.ToString() + "m";
    if (tag == "w") return "\033[38;2;" + Color::White.ToString() + "m";
    if (tag == "error") return "\033[38;2;" + Color::Error.ToString() + "m";
    if (tag == "warn") return "\033[38;2;" + Color::Warning.ToString() + "m";
    if (tag == "info") return "\033[38;2;" + Color::Info.ToString() + "m";
    if (tag == "debug") return "\033[38;2;" + Color::Debug.ToString() + "m";
    if (tag == "reset") return "\033[0m";
    if (tag == "under") return "\033[4m";
    if (tag == "bold") return "\033[1m";

    return "{" + tag + "}";
}

std::string Console::format(const std::string &str, const std::vector<std::string> &args) {
    // Replace all the color tags.
    std::string result;
    result.reserve(str.size()); // Reserve space for optimization

    std::string tagBuffer;
    bool recordingTag = false;
    for (char c: str) {
        if (c == '{') {
            recordingTag = true;
            tagBuffer.clear();
            continue; // skip this char
        } else if (c == '}' && recordingTag) {
            result += replaceTag(tagBuffer);
            recordingTag = false;
            continue; // skip this char
        }

        if (recordingTag) {
            tagBuffer += c;
        } else {
            result += c;
        }
    }

    // Replace all the argument tags.
    for (const auto &arg: args) {
        auto pos = result.find("{}");
        if (pos == std::string::npos) break;

        result.replace(pos, 2, arg);
    }

    return result;
}