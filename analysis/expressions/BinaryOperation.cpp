#include "BinaryOperation.h"
#include "../../compiler/Compiler.h"
#include "../../compiler/TypeCoercion.h"
#include "../../ast/expressions/VarReference.h"
#include "../../ast/expressions/StaticRef.h"
#include "../../ast/literals/Null.h"

void BinaryOperationAnalyzer::Analyze() {
    if (node->op == Token::Type::Assign) {
        analyzeAssign();
        return;
    }

    if (node->op == Token::Type::Is) {
        analyzeIs();
        return;
    }

    node->lhs->Accept(visitor);
    node->rhs->Accept(visitor);

    VisitorResult lhsResult;
    VisitorResult rhsResult;
    if (!visitor->TryGetResult(lhsResult)) return;
    if (!visitor->TryGetResult(rhsResult)) return;

    visitor->AddSuccess(TypeCoercion::getCommonType(lhsResult.type, rhsResult.type));
}

bool BinaryOperationAnalyzer::tryPromoteToVarDecl() {
    auto varRef = dynamic_cast<VariableReference *>(node->lhs);
    if (!varRef) return false;

    // `a.x` is not a valid variable name.
    if (varRef->next) return false;

    // Check if the variable has already been declared. If so, this is a reassignment.
    if (Compiler::getScopeManager().HasVar(varRef->name))
        return false;

    // Promote to a var declaration.
    Compiler::getScopeManager().getContext()->ReplaceCurrentNode(
        new VariableDeclaration(varRef->token, varRef->name, node->rhs)
    );

    visitor->AddSuccess();
    return true;
}

void BinaryOperationAnalyzer::analyzeAssign() {
    if (tryPromoteToVarDecl())
        return;

    // LHS must be a chainable node.
    auto chainNode = dynamic_cast<ChainableNode *>(node->lhs);
    if (!chainNode) {
        visitor->ReportError(ErrorCode::SYNTAX_ERROR, {}, node);
        return;
    }

    VisitorResult lhsResult;
    chainNode->Accept(visitor);
    if (!visitor->TryGetResult(lhsResult)) return;

    VisitorResult rhsResult;
    node->rhs->Accept(visitor);
    if (!visitor->TryGetResult(rhsResult)) return;

    if (!TypeCoercion::isTypeCompatible(lhsResult.type, rhsResult.type)) {
        // TODO: Use a different error based on if the rhs is null or not.
        visitor->ReportError(
            ErrorCode::ASSIGN_TYPE_MISMATCH,
            {
                lhsResult.type->type->name,
                rhsResult.type->type->name
            },
            node
        );
        return;
    }

    visitor->AddSuccess(lhsResult.type);
}

void BinaryOperationAnalyzer::analyzeIs() {
    if (auto chainNode = dynamic_cast<ChainableNode *>(node->lhs)) {
        if (auto null = dynamic_cast<Null *>(node->rhs))
            Compiler::getScopeManager().getContext()->narrowType(chainNode, null->getType());
        else if (auto staticRef = dynamic_cast<StaticRef *>(node->rhs))
            Compiler::getScopeManager().getContext()->narrowType(chainNode, staticRef->getFinalType());
    }

    visitor->AddSuccess(Compiler::getScopeManager().getType("bool"));
}
