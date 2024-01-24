#pragma once

#include "../../ast/Literals.h"
#include "../Parser.h"
#include "../../ast/StringInternTable.h"

String* parseString(Parser* parser) {
    if (parser->currentToken.isNot(Token::Type::String)) {
        parser->PrintSyntaxError("string literal (e.g. \"Forty-two\")");
        return nullptr;
    }

    // Parse escape characters
    std::string str;
    str.reserve(parser->currentToken.value.length() - 2);
    for (size_t i = 1; i < parser->currentToken.value.length() - 1; i++) {
        if (parser->currentToken.value[i] == '\\') {
            switch (parser->currentToken.value[++i]) {
                case 'n':
                    str += '\n';
                    break;
                case 't':
                    str += '\t';
                    break;
                case 'r':
                    str += '\r';
                    break;
                case '0':
                    str += '\0';
                    break;
                case '\\':
                    str += '\\';
                    break;
                case '"':
                    str += '"';
                    break;
                default:
                    parser->PrintSyntaxError("valid escape sequence");
                    return nullptr;
            }
        } else {
            str += parser->currentToken.value[i];
        }
    }

    auto node = new String(
        parser->currentToken,
        StringInternTable::Intern(str)
    );

    parser->NextToken();
    return node;
}
