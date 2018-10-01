#include <stdlib.h>
#include <stdio.h>

#include "token.h"
#include "expr.h"
#include "parser.h"


//TODO: ERROR HANDLING AND PANIC MODE!!!

static BinaryExpr* newBinary(Token op, Expr* left, Expr* right);
static Expr* newExpr(ExpressionType type, void* rExpr);
static Expr* equality(ParsedTokens* PT);
static Expr* comparison(ParsedTokens* PT);
static Expr* addition(ParsedTokens* PT);
static Expr* multiplication(ParsedTokens* PT);
static Expr* unary(ParsedTokens* PT);
static Expr* primary(ParsedTokens* PT);
static Expr* grouping(ParsedTokens* PT);
static Expr* expression(ParsedTokens* PT);
static Token previous(ParsedTokens* PT);

static int match(ParsedTokens* PT, TokenType type, TokenType types[], int typeLen);

int match(ParsedTokens* PT, TokenType type, TokenType types[], int typeLen){
    for(int i = 0; i<typeLen; i++){
        if(MATCH(type, types[i])){
            PT->current++;
            return 1;
        }
    }
    return 0;
}

Token previous(ParsedTokens* PT){
    return PT->tokens[PT->current - 1];
}

Expr* newExpr(ExpressionType type, void* rExpr){
    Expr* expr = malloc(sizeof(Expr));
    expr->type = type;
    expr->expr = rExpr;
    return expr;
}

BinaryExpr* newBinary(Token op, Expr* left, Expr* right){
    BinaryExpr* bExp = malloc(sizeof(BinaryExpr));
    bExp->op = op;
    bExp->left = left;
    bExp->right = right;
    return bExp;
}

UnaryExpr* newUnary(Token op, Expr* right){
    UnaryExpr* UExpr = malloc(sizeof(UnaryExpr));
    UExpr->op = op;
    UExpr->right = right;
    return UExpr;
}

LiteralExpr* newLiteral(LiteralType type, void* value){
   
    LiteralExpr* lExpr = malloc(sizeof(LiteralExpr));
    if(type == LITERAL_NUMBER){
        double* doubleLiteral = (double*)malloc(sizeof(double));
        *doubleLiteral = atof(value);
        lExpr->value = doubleLiteral;    
    }else{
        lExpr->value = value;
    }
    lExpr->type = type;

    return lExpr;
}

GroupingExpr* newGrouping(Expr* expr){
    GroupingExpr* gExpr = malloc(sizeof(GroupingExpr));
    gExpr->expr = expr;
    
    return gExpr;
}

Expr* equality(ParsedTokens* PT){
    Expr* expr = comparison(PT);
    TokenType types[] = {
        BANG_EQUAL,
        EQUAL_EQUAL
    };
    while(match(PT, PT->tokens[PT->current].type, types, 2)){
        Token operator = previous(PT);
        Expr* right = comparison(PT);
        expr = newExpr(EXPR_BINARY, newBinary(operator, expr, right));
    }
    return expr;
} 

Expr* comparison(ParsedTokens* PT){
    Expr* expr = addition(PT);

    TokenType types[] = {
        GREATER,
        GREATER_EQUAL,
        LESS,
        LESS_EQUAL
    };

    while(match(PT, PT->tokens[PT->current].type, types, 4)){
        Token operator = previous(PT);
        Expr* right = addition(PT);
        expr = newExpr(EXPR_BINARY, newBinary(operator, expr, right));
    }
    
    return expr;
}

Expr* addition(ParsedTokens* PT){
    Expr* expr = multiplication(PT);

    TokenType types[] = {
        MINUS,
        PLUS
    };

    while(match(PT, PT->tokens[PT->current].type, types, 2)){
        Token operator = previous(PT);
        Expr* right = multiplication(PT);
        expr = newExpr(EXPR_BINARY, newBinary(operator, expr, right));
    }
    
    return expr;
}

// Expr* addition(ParsedTokens* PT){
//     Expr* expr = multiplication(PT);

//     TokenType types[] = {
//         MINUS,
//         PLUS
//     };

//     while(match(PT, PT->tokens[PT->current].type, types, 2)){
//         Token operator = previous(PT);
//         Expr* right = multiplication(PT);
//         expr = newExpr(EXPR_BINARY, newBinary(operator, expr, right));
//     }
    
//     return expr;
// }

Expr* multiplication(ParsedTokens* PT){
    Expr* expr = unary(PT);

    TokenType types[] = {
        STAR,
        SLASH
    };

    while(match(PT, PT->tokens[PT->current].type, types, 2)){
        Token operator = previous(PT);
        Expr* right = unary(PT);
        expr = newExpr(EXPR_BINARY, newBinary(operator, expr, right));
    }
    
    return expr;
}

Expr* unary(ParsedTokens* PT){
     TokenType types[] = {
        BANG,
        MINUS
    };
    if(match(PT, PT->tokens[PT->current].type, types, 2)){
        Token operator = previous(PT);
        Expr* right = unary(PT);
        return newExpr(EXPR_UNARY, newUnary(operator, right));
    }

    return primary(PT);
}

Expr* primary(ParsedTokens* PT){
    Token token = PT->tokens[PT->current];
    TokenType types[] = { FALSE };
    if(match(PT, token.type, types, 1)){
        return newExpr(EXPR_LITERAL, newLiteral(LITERAL_BOOL, "false"));
    }
    types[0] = TRUE;
    if(match(PT, token.type, types, 1)){
        return newExpr(EXPR_LITERAL, newLiteral(LITERAL_BOOL, "true"));
    }
    types[0] = NULL_L;
    if(match(PT, token.type, types, 1)){
        return newExpr(EXPR_LITERAL, newLiteral(LITERAL_NULL, "null"));
    }
    types[0] = STRING;
    if(match(PT, token.type, types, 1)){
        return newExpr(EXPR_LITERAL, newLiteral(LITERAL_STRING, previous(PT).literal));
    }
    types[0] = NUMBER;
    if(match(PT, token.type, types, 1)){
        return newExpr(EXPR_LITERAL, newLiteral(LITERAL_NUMBER, previous(PT).literal));
    }

    types[0] = LEFT_PAREN;
    if(match(PT, token.type, types, 1)){
        Expr* expr = expression(PT);
        // TODO consume(); parenthsis error handling
        return newExpr(EXPR_GROUPING, newGrouping(expr));
    }
    return NULL;

}

Expr* expression(ParsedTokens* PT){
    return equality(PT);
}



Expr* parseTokens(Tokenizer tokenizer){
    ParsedTokens* PT = malloc(sizeof(ParsedTokens));
    PT->current = 0;
    PT->tokensSize = tokenizer.tokensLen;
    PT->tokens = tokenizer.tokens;

    return expression(PT);



}
