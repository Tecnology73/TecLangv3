#include "ForLoop.h"
#include "../expressions/Expression.h"
#include "../../compiler/Compiler.h"
#include "../../ast/Literals.h"

ForLoop* parseForLoop(Parser* parser) {
    if (parser->currentToken.isNot(Token::Type::For)) {
        parser->PrintSyntaxError("for");
        return nullptr;
    }

    auto loop = new ForLoop(parser->currentToken);
    parser->NextToken(); // Consume 'for'

    // Parse expression (what to loop over, i.e. range, array, etc.)
    loop->value = parseExpression(parser);
    if (!loop->value)
        return nullptr;

    // Parse `as` and `step` (both optional and in any order)
    VariableDeclaration* identifier = nullptr;
    while (parser->currentToken.isNot(Token::Type::OpenCurly)) {
        if (parser->currentToken.is(Token::Type::As)) {
            parser->NextToken(); // Consume 'as'

            if (parser->currentToken.isNot(Token::Type::Identifier)) {
                parser->PrintSyntaxError("identifier");
                return nullptr;
            }

            identifier = new VariableDeclaration(parser->currentToken, parser->currentToken.value.data());
            identifier->type = TypeReference::Infer();

            parser->NextToken(); // Consume identifier
        } else if (parser->currentToken.is(Token::Type::Step)) {
            parser->NextToken(); // Consume 'step'

            loop->step = parseExpression(parser);
            if (!loop->step)
                return nullptr;
        } else {
            parser->PrintSyntaxError("as or step");
            return nullptr;
        }
    }

    // If no identifier was specified, create one.
    if (!identifier) {
        identifier = new VariableDeclaration(loop->token, StringInternTable::Intern("it"));
        // identifier->type = TypeReference::Infer();
        identifier->type = std::get<TypeDefinition*>(SymbolTable::GetInstance()->Get("i32")->value)->CreateReference();
        // We default to zero here.
        // We'll set this to the actual start expression when we generate the for loop.
        identifier->expression = new Integer(loop->token, 0);
    }
    loop->identifier = identifier;

    if (parser->currentToken.isNot(Token::Type::OpenCurly)) {
        parser->PrintSyntaxError("{");
        return nullptr;
    }

    // Parse Body
    parser->NextToken(); // Consume '{'
    while (parser->currentToken.isNot(Token::Type::CloseCurly)) {
        auto expr = parseExpression(parser);
        if (!expr) return nullptr;

        loop->body.push_back(expr);
    }

    parser->NextToken(); // Consume '}'
    return loop;
}
