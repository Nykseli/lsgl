#include "visitor.h"

void* acceptExpr(ExpressionVisitor visitor, Expr* expr){
    switch (expr->type) {
        case EXPR_LITERAL:
            return visitor.visitLiteral(expr->expr);
        case EXPR_UNARY:
            return visitor.visitUnary(expr->expr);
        case EXPR_BINARY:
            return visitor.visitBinary(expr->expr);
        case EXPR_GROUPING:
            return visitor.visitGrouping(expr->expr);
    }
}

