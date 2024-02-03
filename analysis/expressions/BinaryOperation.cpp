#include "BinaryOperation.h"

#include "../../ast/expressions/FunctionCall.h"
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

void BinaryOperationAnalyzer::analyzeAssign() {
    // LHS must be a chainable node.
    auto chainNode = dynamic_cast<ChainableNode *>(node->lhs);
    if (!chainNode) {
        visitor->ReportError(ErrorCode::SYNTAX_ERROR, {}, node);
        return;
    }

    VisitorResult lhsResult;
    chainNode->Accept(visitor);
    if (!visitor->TryGetResult(lhsResult)) return;

    // TODO: Support other types (like arrays) that proxy to their internal data.
    // If the right-most node (of LHS) is a string, we do not want to load it.
    // We have special `T_assign` functions that take in the struct as the first argument.
    if (chainNode->getFinalType()->name == "string")
        chainNode->loadInternalData = false;

    VisitorResult rhsResult;
    node->rhs->Accept(visitor);
    if (!visitor->TryGetResult(rhsResult)) return;

    if (!TypeCoercion::isTypeCompatible(lhsResult.type, rhsResult.type)) {
        // TODO: Use a different error based on if the rhs is null or not.
        visitor->ReportError(
            ErrorCode::ASSIGN_TYPE_MISMATCH,
            {
                lhsResult.type->name,
                rhsResult.type->name
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
            Compiler::getScopeManager().getContext()->narrowType(
                chainNode,
                staticRef->getFinalType()->CreateReference()
            );
    }

    visitor->AddSuccess(SymbolTable::GetInstance()->GetReference("bool"));
}
