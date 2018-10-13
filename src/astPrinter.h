#ifndef _LSGL_PRINT_AST_H_
#define _LSGL_PRINT_AST_H_
#include "expr.h"

void* visitBinary(void* bExpr);
void* visitUnary(void* expr);
void* visitGrouping(void* expr);
void* visitLiteral(void* expr);

void printAst(Expr* expr);
#endif
