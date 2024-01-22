#include "Console.h"

void Console::print(const std::string& message, const std::vector<std::string>& args) {
    std::cout << format(message + "{reset}", args) << std::endl;
}

void Console::printRaw(const std::string& message) {
    std::cout << message << std::endl;
}

std::string Console::replaceTag(const std::string& tag) {
    if (tag == "r") return "\033[38;2;" + Color::Red.ToString() + "m";
    if (tag == "g") return "\033[38;2;" + Color::Green.ToString() + "m";
    if (tag == "b") return "\033[38;2;" + Color::Blue.ToString() + "m";
    if (tag == "o" || tag == "keyword") return "\033[38;2;" + Color::Orange.ToString() + "m";
    if (tag == "y") return "\033[38;2;" + Color::Gray.ToString() + "m";
    if (tag == "w") return "\033[38;2;" + Color::White.ToString() + "m";
    if (tag == "Y" || tag == "type") return "\033[38;2;" + Color::Yellow.ToString() + "m";
    if (tag == "error") return "\033[38;2;" + Color::Error.ToString() + "m";
    if (tag == "warn") return "\033[38;2;" + Color::Warning.ToString() + "m";
    if (tag == "info") return "\033[38;2;" + Color::Info.ToString() + "m";
    if (tag == "debug") return "\033[38;2;" + Color::Debug.ToString() + "m";
    if (tag == "reset") return "\033[0m";
    if (tag == "under") return "\033[4m";
    if (tag == "bold") return "\033[1m";

    return "{" + tag + "}";
}

std::string Console::format(const std::string& str, const std::vector<std::string>& args) {
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
    for (const auto& arg: args) {
        auto pos = result.find("{}");
        if (pos == std::string::npos) break;

        result.replace(pos, 2, arg);
    }

    return result;
}

std::string Console::OpToString(Token::Type op) {
    switch (op) {
        case Token::Type::Identifier:
            return "Identifier";
        case Token::Type::Integer:
            return "Integer";
        case Token::Type::Double:
            return "Double";
        case Token::Type::String:
            return "String";
        case Token::Type::Punctuation:
            return "Punctuation";
        case Token::Type::EndOfFile:
            return "EndOfFile";
        case Token::Type::Unknown:
            return "Unknown";

        // Keywords
        case Token::Type::Function:
            return "Function";
        case Token::Type::Return:
            return "Return";
        case Token::Type::If:
            return "If";
        case Token::Type::Else:
            return "Else";
        case Token::Type::Boolean_True:
            return "Boolean_True";
        case Token::Type::Boolean_False:
            return "Boolean_False";
        case Token::Type::For:
            return "For";
        case Token::Type::Continue:
            return "Continue";
        case Token::Type::Break:
            return "Break";
        case Token::Type::In:
            return "In";
        case Token::Type::Type:
            return "Type";
        case Token::Type::New:
            return "New";
        case Token::Type::When:
            return "When";
        case Token::Type::Extern:
            return "Extern";
        case Token::Type::As:
            return "As";
        case Token::Type::Step:
            return "Step";
        case Token::Type::Enum:
            return "Enum";
        case Token::Type::Public:
            return "Public";
        case Token::Type::Private:
            return "Private";
        case Token::Type::Is:
            return "Is";
        case Token::Type::Null:
            return "Null";

        // Symbols
        case Token::Type::OpenParen:
            return "OpenParen";
        case Token::Type::CloseParen:
            return "CloseParen";
        case Token::Type::OpenCurly:
            return "OpenCurly";
        case Token::Type::CloseCurly:
            return "CloseCurly";
        case Token::Type::OpenBracket:
            return "OpenBracket";
        case Token::Type::CloseBracket:
            return "CloseBracket";
        case Token::Type::Comma:
            return "Comma";
        case Token::Type::Colon:
            return "Colon";
        case Token::Type::ColonColon:
            return "ColonColon";
        case Token::Type::Semicolon:
            return "Semicolon";
        case Token::Type::Period:
            return "Period";
        case Token::Type::Spread:
            return "Spread";
        case Token::Type::TripleDots:
            return "TripleDots";
        case Token::Type::Assign:
            return "Assign";
        case Token::Type::Plus:
            return "Plus";
        case Token::Type::PlusPlus:
            return "PlusPlus";
        case Token::Type::Minus:
            return "Minus";
        case Token::Type::MinusMinus:
            return "MinusMinus";
        case Token::Type::Multiply:
            return "Multiply";
        case Token::Type::Divide:
            return "Divide";
        case Token::Type::BackSlash:
            return "BackSlash";
        case Token::Type::Percent:
            return "Percent";
        case Token::Type::Ampersand:
            return "Ampersand";
        case Token::Type::Pipe:
            return "Pipe";
        case Token::Type::Caret:
            return "Caret";
        case Token::Type::Exclamation:
            return "Exclamation";
        case Token::Type::Tilde:
            return "Tilde";
        case Token::Type::Question:
            return "Question";
        case Token::Type::LessThan:
            return "LessThan";
        case Token::Type::GreaterThan:
            return "GreaterThan";
        case Token::Type::LessThanOrEqual:
            return "LessThanOrEqual";
        case Token::Type::GreaterThanOrEqual:
            return "GreaterThanOrEqual";
        case Token::Type::Equal:
            return "Equal";
        case Token::Type::NotEqual:
            return "NotEqual";
        case Token::Type::LogicalAnd:
            return "LogicalAnd";
        case Token::Type::LogicalOr:
            return "LogicalOr";
        case Token::Type::PlusEqual:
            return "PlusEqual";
        case Token::Type::MinusEqual:
            return "MinusEqual";
        case Token::Type::MultiplyEqual:
            return "MultiplyEqual";
        case Token::Type::DivideEqual:
            return "DivideEqual";
        case Token::Type::PercentEqual:
            return "PercentEqual";
        case Token::Type::AmpersandEqual:
            return "AmpersandEqual";
        case Token::Type::CaretEqual:
            return "CaretEqual";
        case Token::Type::PipeEqual:
            return "PipeEqual";
        case Token::Type::Arrow:
            return "Arrow";
    }

    return "Unknown (" + std::to_string((int) op) + ")";
}
