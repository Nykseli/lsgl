#ifndef _LSGL_VISITOR_H_
#define _LSGL_VISITOR_H_
#include "expr.h"

typedef void* (*ActionExpr)(Expr*);

typedef struct expression_visitor {
    ActionExpr visitBinary;
    ActionExpr visitUnary;
    ActionExpr visitLiteral;
    ActionExpr visitGrouping;
    ActionExpr visitVar;
    ActionExpr visitAssign;
} ExpressionVisitor;

void* acceptExpr(ExpressionVisitor visitor, Expr* expr);

typedef void* (*ActionStmt)(Stmt*);

typedef struct stmt_visitor {
    ActionStmt visitPrint;
    ActionStmt visitExpression;
    ActionStmt visitVar;
    ActionStmt visitBlock;
} StatementVisitor;

void* acceptStmt(StatementVisitor visitor, Stmt* stmt);
//     R visitGroupingExpr(Grouping expr);
//     R visitLiteralExpr(Literal expr);
//     R visitUnaryExpr(Unary expr);
// R visitBinaryExpr(Binary expr);
#endif
