#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "expr.h"
#include "dict.h"
#include "visitor.h"

static void* visitLiteralExpr(void* expr);
static void* visitGroupingExpr(void* expr);
static void* visitUnaryExpr(void* expr);
static void* visitBinaryExpr(void* expr);
static void* visitVarExpr(void* expr);

static char* isTruthy(LiteralExpr* expr);

static void* visitExpressionStmt(Stmt* stmt);
static void* visitPrintStmt(Stmt* stmt);
static void* visitVarStmt(Stmt* stmt);
static void execute(Stmt* stmt);

static void* evaluate(Expr* expr);

static Dictionary* enviromentDict;

ExpressionVisitor EvalExpressionVisitor = {
    .visitLiteral = visitLiteralExpr,
    .visitGrouping = visitGroupingExpr,
    .visitUnary = visitUnaryExpr,
    .visitBinary = visitBinaryExpr,
    .visitVar = visitVarExpr
};

StatementVisitor EvalStatmentVisitor = {
    .visitExpression = visitExpressionStmt,
    .visitPrint = visitPrintStmt,
    .visitVar = visitVarStmt
};

static char* isNonTruthy(LiteralExpr* expr){
    if(expr->type == NULL_L) return FALSE_KEY;

    if(expr->type == LITERAL_BOOL){
        strcmp(TRUE_KEY, (const char*)expr->value) != 0 ? TRUE_KEY : FALSE_KEY;
    }

    return FALSE_KEY;
}

static int isEqual(LiteralExpr* left, LiteralExpr* right){
    if(left->type == LITERAL_NULL && right->type == LITERAL_NULL){
        return 0;
    }else if(left->type == LITERAL_STRING && right->type == LITERAL_STRING){
        return strcmp((char*)left->value, (const char*)right->value) != 0 ? 1 : 0;
    }else if(left->type == LITERAL_NUMBER && right->type == LITERAL_NUMBER){
        if( *((double*)left->value) == *((double*)right->value)) return  0;
        return  1;
    }


    return 1;
}

static void* evaluate(Expr* expr){
    return acceptExpr(EvalExpressionVisitor, expr);
}

static void* visitLiteralExpr(void* expr){
    const LiteralExpr* lExpr = (LiteralExpr*)expr;
    return lExpr;
}

static void* visitGroupingExpr(void* expr){
    const GroupingExpr* gExpr = (GroupingExpr*)expr;
    return evaluate(gExpr);

}
static void* visitUnaryExpr(void* expr){
    const UnaryExpr* uExpr = (UnaryExpr*)expr;
    char* truthy = NULL;
    LiteralExpr* right = evaluate(uExpr->right);
    double* value = malloc(sizeof(double));
    switch(uExpr->op.type){
        case MINUS:
            value = right->value;
            *value = -*value;
            right->value = value;
            break;
        case BANG:
            truthy = isNonTruthy(right);
            right->type = LITERAL_BOOL;
            right->value = truthy;
            break;        

    }
    return right;

}
static void* visitBinaryExpr(void* expr){
    const BinaryExpr* bExpr = (BinaryExpr*)expr;
    LiteralExpr* right = (LiteralExpr*)evaluate(bExpr->right);
    LiteralExpr* left = (LiteralExpr*)evaluate(bExpr->left);
    LiteralExpr* result = malloc(sizeof(LiteralExpr));

    char* resultStr;
    double* value = malloc(sizeof(double*));
    int valueLength;
    double leftValue, rightValue;

    switch(bExpr->op.type){
        case MINUS:
            if(right->type == LITERAL_NUMBER && left->type == LITERAL_NUMBER){
                result->type = LITERAL_NUMBER;
                *value = *((double*)left->value) - *((double*)right->value);
                result->value = value;
                break;
            }else{
                //TODO: ERROR
            }
        case PLUS:
            //TODO support plus between strings and numbers
            if(right->type == LITERAL_NUMBER && left->type == LITERAL_NUMBER){
                result->type = LITERAL_NUMBER;
                *value = *((double*)left->value) + *((double*)right->value);
                result->value = value;
                break;
            }else if(right->type == LITERAL_STRING && left->type == LITERAL_STRING){
                valueLength = strlen((char*) right->value) + strlen((char*) left->value);
                valueLength++;
                resultStr = malloc(sizeof(char) * valueLength);
                strcpy(resultStr, (char*) left->value);
                strcat(resultStr, (char*) right->value);
                result->value = resultStr;
                result->valueSize = valueLength;
                result->type = LITERAL_STRING;
            }else{
                //TODO: ERROR
            }
        case GREATER:
            if(right->type == LITERAL_NUMBER && left->type == LITERAL_NUMBER){
                result->type = LITERAL_BOOL;
                leftValue = *((double*)left->value);
                rightValue = *((double*)right->value);
                result->value = leftValue > rightValue ? TRUE_KEY : FALSE_KEY;
            }else{
                //TODO: ERROR
            }
            break;
        case GREATER_EQUAL:
            if(right->type == LITERAL_NUMBER && left->type == LITERAL_NUMBER){
                result->type = LITERAL_BOOL;
                leftValue = *((double*)left->value);
                rightValue = *((double*)right->value);
                result->value = leftValue >= rightValue ? TRUE_KEY : FALSE_KEY;
            }else{
                //TODO: ERROR
            }
            break;
        case LESS:
            if(right->type == LITERAL_NUMBER && left->type == LITERAL_NUMBER){
                result->type = LITERAL_BOOL;
                leftValue = *((double*)left->value);
                rightValue = *((double*)right->value);
                result->value = leftValue < rightValue ? TRUE_KEY : FALSE_KEY;
            }else{
                //TODO: ERROR
            }
            break;
        case LESS_EQUAL:
            if(right->type == LITERAL_NUMBER && left->type == LITERAL_NUMBER){
                result->type = LITERAL_BOOL;
                leftValue = *((double*)left->value);
                rightValue = *((double*)right->value);
                result->value = leftValue <= rightValue ? TRUE_KEY : FALSE_KEY;
            }else{
                //TODO: ERROR
            }
            break;
        case EQUAL_EQUAL:
            result->type = LITERAL_BOOL;
            if(isEqual(left, right) == 0){
                result->value = TRUE_KEY;
            }else{
                result->value = FALSE_KEY;
            }
            break;
        case BANG_EQUAL:
            result->type = LITERAL_BOOL;
            if(isEqual(left, right) != 0){
                result->value = TRUE_KEY;
            }else{
                result->value = FALSE_KEY;
            }
            break;
    }

    return result;

}

static void* visitVarExpr(void* expr){
    VarExpr* vExpr = (VarExpr*) expr;
    return dictGet(enviromentDict, vExpr->name.lexeme);
}

static void* visitExpressionStmt(Stmt* stmt){
    LiteralExpr* lExpr = (LiteralExpr*)evaluate(stmt->expr);
    return NULL;
}

static void* visitPrintStmt(Stmt* stmt){
    
    LiteralExpr* lExpr = (LiteralExpr*)evaluate(stmt->expr);
    
    if(lExpr->type == NULL) return NULL;

    if(lExpr->type == LITERAL_STRING){
        printf("Interpret string: %s\n", (char*) lExpr->value);
    }else if(lExpr->type == LITERAL_NUMBER){
        printf("Interpret double: %f\n", *((double*) lExpr->value));
    }else if(lExpr->type == LITERAL_BOOL){
        printf("Interpret boolean: %s\n", (char*) lExpr->value);
    }
}

static void* visitVarStmt(Stmt* stmt){
    LiteralExpr* lExpr = (LiteralExpr*)evaluate(stmt->expr);
    
    dictAdd(enviromentDict, stmt->name.lexeme, lExpr);
    
}

static void execute(Stmt* stmt){
    acceptStmt(EvalStatmentVisitor, stmt);
}

void interpret(ParsedStmt stmts){
    enviromentDict = dict();
    int i;
    for(i = 0; i < stmts.stmtLen; i++){
        execute(&stmts.stmt[i]);
    }
}
