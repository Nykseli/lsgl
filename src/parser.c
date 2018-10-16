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
static Expr* variable(ParsedTokens* PT);
static Expr* assignment(ParsedTokens* PT);
static Token previous(ParsedTokens* PT);

static Stmt expressionStatement(ParsedTokens* PT);
static Stmt printStatement(ParsedTokens* PT);
static Stmt varStatement(Expr* initializer, Token name);
static Stmt statement(ParsedTokens* PT);
static Stmt varDeclaration(ParsedTokens* PT);
static Stmt declaration(ParsedTokens* PT);
static Stmt blockStatment(ParsedTokens* PT);

static void error(ParsedTokens* PT, const char* msg);
static Token consume(ParsedTokens* PT, TokenType type, const char* msg);

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

static void error(ParsedTokens* PT, const char* msg){
    Token token = PT->tokens[PT->current];
    if (token.type == EOF) {
        except("Syntax Error at end of file: %s\n", msg);
    } else {
        except("Syntax Error (Line %d): %s '%s'\n", token.line, msg, token.lexeme);
    }
}

static Token consume(ParsedTokens* PT, TokenType type, const char* msg){
    Token token = PT->tokens[PT->current];
    if (MATCH(token.type, type)) {
        PT->current++;
        return token;
    }
    token.lexeme = NULL;
    error(PT, msg);
    return token;
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

VarExpr* newVariable(Token value){
    VarExpr* varExpr = malloc(sizeof(VarExpr));
    varExpr->name = value;

    return varExpr;
}

AssignExpr* newAssign(Token name, Expr* value){
    AssignExpr* aExpr = malloc(sizeof(AssignExpr));
    aExpr->name = name;
    aExpr->value = value;
    return aExpr;
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
    types[0] = IDENTIFIER;
    if(match(PT, token.type, types, 1)){
        return newExpr(EXPR_VARIABLE, newVariable(previous(PT))); 
    } 

    types[0] = LEFT_PAREN;
    if(match(PT, token.type, types, 1)){
        Expr* expr = expression(PT);
        // TODO consume(); parenthsis error handling
        return newExpr(EXPR_GROUPING, newGrouping(expr));
    }   
    return NULL;

}

Expr* assignment(ParsedTokens* PT){
    Expr* eExpr = equality(PT);

    Token types[] = { EQUAL }; 
    if(match(PT, PT->tokens[PT->current].type, types, 1)){
        Token equal = previous(PT);
        Expr* value = assignment(PT);

        if(eExpr->type == EXPR_VARIABLE){
            VarExpr* varExpr = eExpr->expr;
            Token name = varExpr->name;
            return newExpr(EXPR_ASSIGNMENT, newAssign(name, value));
        }
        error(PT, "Invalid assignment target.");
    }
    return eExpr;
}

Expr* expression(ParsedTokens* PT){
    return assignment(PT);
}



Stmt printStatement(ParsedTokens* PT){
    Stmt stmt;
    PrintStmt* pStmt = malloc(sizeof(PrintStmt));
    pStmt->expr = expression(PT);
    stmt.type = STMT_PRINT;
    stmt.stmt = pStmt;
    consume(PT, SEMICOLON, "Expect ';' after value.");
    return stmt;
}

Stmt expressionStatement(ParsedTokens* PT){
    Stmt stmt;
    ExprStmt* eStmt = malloc(sizeof(ExprStmt));
    eStmt->expr = expression(PT);
    stmt.type = STMT_EXPR;
    stmt.stmt = eStmt;
    consume(PT, SEMICOLON, "Expect ';' after value.");
    return stmt;
}

Stmt varStatement(Expr* initializer, Token name){
    Stmt stmt;
    VarStmt* vStmt = malloc(sizeof(VarStmt));
    vStmt->expr = initializer;
    vStmt->name = name;
    stmt.type = STMT_VAR;
    stmt.stmt = vStmt;
    return stmt;
}
Stmt blockStatment(ParsedTokens* PT){
    Stmt stmt;
    BlockStmt* bStmt = malloc(sizeof(VarStmt));
    bStmt->statements = NULL;
    bStmt->stmtLen = 0;

    while(!MATCH(PT->tokens[PT->current].type, RIGHT_BRACE)){
        bStmt->statements = realloc(bStmt->statements, sizeof(Stmt) * (bStmt->stmtLen + 1));
        bStmt->statements[bStmt->stmtLen] = declaration(PT);
        bStmt->stmtLen++;
    }

    stmt.type = STMT_BLOCK;
    stmt.stmt = bStmt;
    consume(PT, RIGHT_BRACE, "Expect '}' after block.");
    
    return stmt;
}

Stmt statement(ParsedTokens* PT) {
    Token token = PT->tokens[PT->current];
    if(MATCH(token.type, PRINT)){
        PT->current++;
        return printStatement(PT);
    }
    if(MATCH(token.type, LEFT_BRACE)){
        PT->current++;
        return blockStatment(PT);
    }

    return expressionStatement(PT);
}

Stmt varDeclaration(ParsedTokens* PT) {
    Token name = consume(PT, IDENTIFIER, "Expected variable name.");
    
    Expr* initializer = NULL;
    TokenType types[] = { EQUAL };
    if(match(PT, PT->tokens[PT->current].type, types, 1)){
        initializer = expression(PT);
    }

    consume(PT, SEMICOLON, "Expect ';' after variable declaration.");
    
    return varStatement(initializer, name);
}

Stmt declaration(ParsedTokens* PT) {
    Token token = PT->tokens[PT->current];
    if(MATCH(token.type, VAR)){
        PT->current++;
        return varDeclaration(PT);
    }
    return statement(PT);
}

ParsedStmt parseTokens(Tokenizer tokenizer){
    ParsedStmt PS;
    PS.stmtLen = 0;
    PS.stmt = NULL;

    ParsedTokens* PT = malloc(sizeof(ParsedTokens));
    PT->current = 0;
    PT->tokensSize = tokenizer.tokensLen;
    PT->tokens = tokenizer.tokens;

    while(PT->current < tokenizer.tokensLen){
        PS.stmt = realloc(PS.stmt, sizeof(Stmt) * (PS.stmtLen + 1));
        PS.stmt[PS.stmtLen] = declaration(PT);
        PS.stmtLen++;
    }
    
    return PS;

}
