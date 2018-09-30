#include "resolve.h"
#include "visitor.h"
#include "stdio.h"

static int resolveExpr(Expr* expr);
static void* visitBinaryExprResolver(Expr* expr);
static void* visitUnaryExprResolver(Expr* expr);
static void* visitLiteralExprResolver(Expr* expr);
static void* visitGroupingExprResolver(Expr* expr);

ExpressionVisitor ExpressionResolver = {
    visitBinaryExprResolver,
    visitUnaryExprResolver,
    visitLiteralExprResolver,
    visitGroupingExprResolver,
};


static int resolveExpr(Expr* expr){
    return acceptExpr(ExpressionResolver, expr) != NULL;
}

static void* visitBinaryExprResolver(Expr* expr){
    BinaryExpr* binary = (BinaryExpr*) expr->expr;
    return !resolveExpr(binary->left) || !resolveExpr(binary->right) ? NULL : expr;
}

static void* visitUnaryExprResolver(Expr* expr){
    UnaryExpr* unary = (UnaryExpr*) expr->expr;
    return !resolveExpr(unary->right) ? NULL : expr;
}

static void* visitLiteralExprResolver(Expr* expr){
    LiteralExpr* literal = (LiteralExpr*) expr->expr;
    return !resolveExpr(literal->value) ? NULL : expr;
}

static void* visitGroupingExprResolver(Expr* expr){
    GroupingExpr* grouping = (GroupingExpr*) expr->expr;
    return !resolveExpr(grouping->expr) ? NULL : expr;
}


