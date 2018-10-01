#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "expr.h"
#include "visitor.h"

static void* visitLiteralExpr(void* expr);
static void* visitGroupingExpr(void* expr);
static void* visitUnaryExpr(void* expr);
static void* visitBinaryExpr(void* expr);

static char* isTruthy(LiteralExpr* expr);

static void* evaluate(Expr* expr);

ExpressionVisitor EvalExpressionVisitor = {
    .visitLiteral = visitLiteralExpr,
    .visitGrouping = visitGroupingExpr,
    .visitUnary = visitUnaryExpr,
    .visitBinary = visitBinaryExpr
};

static char* isTruthy(LiteralExpr* expr){
    if(expr->type == NULL_L) return FALSE_KEY;

    if(expr->type == LITERAL_BOOL){
        strcmp(TRUE_KEY, (const char*)expr->value) != 0 ? TRUE_KEY : FALSE_KEY;
    }

    return FALSE_KEY;
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
            truthy = isTruthy(right);
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
    int valueLength, leftLen, rightLen;

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

            
    }

    return result;

}




void interpret(Expr* astTree){
    LiteralExpr* lExpr = (LiteralExpr*)evaluate(astTree);
    
    if(lExpr->type == NULL) return;

    if(lExpr->type == LITERAL_STRING){
        printf("Interpret string: %s\n", (char*) lExpr->value);
    }else if(lExpr->type == LITERAL_NUMBER){
        printf("Interpret double: %f\n", *((double*) lExpr->value));
    }
}