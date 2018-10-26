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
        case EXPR_VARIABLE:
            return visitor.visitVar(expr->expr);
        case EXPR_ASSIGNMENT:
            return visitor.visitAssign(expr->expr);
        case EXPR_LOGICAL:
            return visitor.visitLogical(expr->expr);
        case EXPR_CALL:
            return visitor.visitCall(expr->expr);
    }
}

void* acceptStmt(StatementVisitor visitor, Stmt* stmt){
    switch(stmt->type){
        case STMT_EXPR:
            return visitor.visitExpression(stmt);
        case STMT_PRINT:
            return visitor.visitPrint(stmt);
        case STMT_VAR:
            return visitor.visitVar(stmt);
        case STMT_BLOCK:
            return visitor.visitBlock(stmt);
        case STMT_IF:
            return visitor.visitIf(stmt);
        case STMT_WHILE:
            return visitor.visitWhile(stmt);
        case STMT_FUNCTION:
            return visitor.visitFunction(stmt);
    }
}
