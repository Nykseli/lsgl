#ifndef _LOX_VISITOR_H_
#define _LOX_VISITOR_H_
#include "expr.h"

typedef void* (*ActionExpr)(Expr*);

typedef struct expression_visitor {
    ActionExpr visitBinary;
    ActionExpr visitUnary;
    ActionExpr visitLiteral;
    ActionExpr visitGrouping;
} ExpressionVisitor;

void* acceptExpr(ExpressionVisitor visitor, Expr* expr);

//     R visitGroupingExpr(Grouping expr);
//     R visitLiteralExpr(Literal expr);
//     R visitUnaryExpr(Unary expr);
// R visitBinaryExpr(Binary expr);
#endif
