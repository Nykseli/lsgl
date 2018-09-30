#include <stdio.h>
#include "astPrinter.h"
#include "visitor.h"

static void* evaluate(Expr* expr);
ExpressionVisitor EvaluateExpressionVisitor = {
    .visitLiteral = visitLiteral,
    .visitGrouping = visitGrouping,
    .visitUnary = visitUnary,
    .visitBinary = visitBinary
};


static void* evaluate(Expr* expr){
    return acceptExpr(EvaluateExpressionVisitor, expr);
}

void* visitBinary(void* expr){
    BinaryExpr* bExpr = (BinaryExpr*)expr;
    if(bExpr->left != NULL)
        evaluate(bExpr->left);
    if(bExpr->left != NULL)
        evaluate(bExpr->right);
    printf("visitBinary\n");
}

void* visitUnary(void* expr){
    
    printf("visitUnary\n");
}

void* visitGrouping(void* expr){
    printf("visitGrouping\n");
    
}

void* visitLiteral(void* expr){
    printf("visitLiteral\n");
    
}

void printAst(Expr* expr){
    evaluate(expr);
}
