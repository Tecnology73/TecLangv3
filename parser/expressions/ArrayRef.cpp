#include "ArrayRef.h"
#include "../../ast/StringInternTable.h"

ArrayRef* parseArrayRef(Parser* parser) {
    if (parser->currentToken.isNot(Token::Type::OpenBracket)) {
        parser->PrintSyntaxError("[");
        return nullptr;
    }

    if (parser->NextToken().isNot(Token::Type::Integer)) {
        parser->PrintSyntaxError("array index");
        return nullptr;
    }

    auto node = new ArrayRef(
        parser->currentToken,
        StringInternTable::Intern(std::to_string(parser->currentToken.intValue))
    );
    node->index = parser->currentToken.intValue;

    if (parser->NextToken().isNot(Token::Type::CloseBracket)) {
        parser->PrintSyntaxError("]");
        return nullptr;
    }

    parser->NextToken(); // Consume ']'

    return node;
}
