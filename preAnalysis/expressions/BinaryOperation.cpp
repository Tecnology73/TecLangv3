#include "BinaryOperation.h"
#include "../../ast/expressions/VarReference.h"
#include "../../compiler/Compiler.h"
#include "../../context/preAnalysis/FunctionContext.h"
#include "../../symbolTable/SymbolTable.h"

bool tryPromoteToVarDecl(Visitor* visitor, const BinaryOperation* node) {
    if (node->op != Token::Type::Assign) return false;

    auto varRef = dynamic_cast<VariableReference *>(node->lhs);
    if (!varRef) return false;

    // `a.x` is not a valid variable name.
    // This is an assignment to a type field.
    if (varRef->next) return false;

    // Check if the variable has already been declared. If so, this is a re-assignment.
    if (Compiler::getScopeManager().HasVar(varRef->name))
        return false;

    // Promote to a var declaration.
    Compiler::getScopeManager().getContext()->ReplaceCurrentNode(
        new VariableDeclaration(varRef->token, varRef->name, node->rhs)
    );

    visitor->AddSuccess();
    return true;
}

void analyseBinaryOperation(Visitor* visitor, BinaryOperation* node) {
    if (tryPromoteToVarDecl(visitor, node)) return;
}
