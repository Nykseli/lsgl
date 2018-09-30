#ifndef _LOX_PRINT_AST_H
#define _LOX_PRINT_AST_H
#include "expr.h"

void* visitBinary(void* bExpr);
void* visitUnary(void* expr);
void* visitGrouping(void* expr);
void* visitLiteral(void* expr);

void printAst(Expr* expr);
#endif
