#include <stdlib.h>
#include <stdio.h>

#include "token.h"
#include "expr.h"
#include "parser.h"


//TODO: ERROR HANDLING AND PANIC MODE!!!

static BinaryExpr* newBinary(Token op, Expr* left, Expr* right);
static LogicalExpr* newLogical(Token op, Expr* left, Expr* right);
static CallExpr* newCall(Token paren, Expr* callee, Expr** arguments, int argLen);
static Expr* newExpr(ExpressionType type, void* rExpr);
static Expr* equality(ParsedTokens* PT);
static Expr* comparison(ParsedTokens* PT);
static Expr* addition(ParsedTokens* PT);
static Expr* multiplication(ParsedTokens* PT);
static Expr* unary(ParsedTokens* PT);
static Expr* finnishCall(ParsedTokens* PT, Expr* expr);
static Expr* call(ParsedTokens* PT);
static Expr* primary(ParsedTokens* PT);
static Expr* grouping(ParsedTokens* PT);
static Expr* expression(ParsedTokens* PT);
static Expr* variable(ParsedTokens* PT);
static Expr* assignment(ParsedTokens* PT);
static Expr* or(ParsedTokens* PT);
static Expr* and(ParsedTokens* PT);
static Token previous(ParsedTokens* PT);

static Stmt* newStatement(StmtType type, void* stmt);
static Stmt* expressionStatement(ParsedTokens* PT);
static Stmt* functionStatment(ParsedTokens* PT);
static Stmt* printStatement(ParsedTokens* PT);
static Stmt* varStatement(Expr* initializer, Token name);
static Stmt* statement(ParsedTokens* PT);
static Stmt* varDeclaration(ParsedTokens* PT);
static Stmt* declaration(ParsedTokens* PT);
static Stmt* blockStatment(ParsedTokens* PT);
static Stmt* ifStatment(ParsedTokens* PT);
static Stmt* whileStatment(ParsedTokens* PT);
static Stmt* forStatment(ParsedTokens* PT);

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

CallExpr* newCall(Token paren, Expr* callee, Expr** arguments, int argLen){
    CallExpr* cExpr = malloc(sizeof(CallExpr));
    cExpr->paren = paren;
    cExpr->callee = callee;
    cExpr->arguments = arguments;
    cExpr->argLen = argLen;

    return cExpr;
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

LogicalExpr* newLogical(Token op, Expr* right, Expr* left){
    LogicalExpr* loExpr = malloc(sizeof(LogicalExpr));
    loExpr->op = op;
    loExpr->right = right;
    loExpr->left = left;

    return loExpr;
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

    return call(PT);
}

Expr* finnishCall(ParsedTokens* PT, Expr* callee){
    Expr** arguments = NULL;
    int argLen = 0;
    TokenType types[] = {COMMA};
    if(!MATCH(PT->tokens[PT->current].type, RIGHT_PAREN)){
        do {
            //TODO: I think we need more than 16 arguments but this will suffice for now
            if(argLen >= 16){
                error(PT, "Cannot have more than 16 arguments.");
            }
            arguments = realloc(arguments, sizeof(Expr) * argLen + 1);
            arguments[argLen] = expression(PT);
            argLen++;
            
        }while(match(PT, PT->tokens[PT->current].type, types, 1));
    }

    Token paren = consume(PT, RIGHT_PAREN, "Expect ')' after arguments.");

    return newExpr(EXPR_CALL, newCall(paren, callee, arguments, argLen));
}

Expr* call(ParsedTokens* PT){
    Expr* expr = primary(PT);

    //TODO: while loop makes this this wierd function call possible
    // foo()()() and i dont know if i want that
    while(1){
        if(MATCH(PT->tokens[PT->current].type, LEFT_PAREN)){
            PT->current++;
            expr = finnishCall(PT, expr);
        }else{
            return expr;
        }
    }

    return expr;
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
    Expr* eExpr = or(PT);

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

Expr* or(ParsedTokens* PT){
    Expr* expr = and(PT);

   TokenType types[] = {OR};
    while(match(PT, PT->tokens[PT->current].type, types, 1)){
        Token operator = previous(PT);
        Expr* right = and(PT);
        return newExpr(EXPR_LOGICAL, newLogical(operator, right, expr));
    }

    return expr;
}

Expr* and(ParsedTokens* PT){
    Expr* expr = equality(PT);

   TokenType types[] = {AND};
    while(match(PT, PT->tokens[PT->current].type, types, 1)){
        Token operator = previous(PT);
        Expr* right = equality(PT);
        return newExpr(EXPR_LOGICAL, newLogical(operator, right, expr));
    }

    return expr;
}

Expr* expression(ParsedTokens* PT){
    return assignment(PT);
}


Stmt* newStatement(StmtType type, void* stmt){
    Stmt* nStmt = malloc(sizeof(Stmt));
    nStmt->type = type;    
    nStmt->stmt = stmt;    
    return nStmt;
}

Stmt* printStatement(ParsedTokens* PT){
    Stmt* stmt = malloc(sizeof(Stmt));
    PrintStmt* pStmt = malloc(sizeof(PrintStmt));
    pStmt->expr = expression(PT);
    stmt->type = STMT_PRINT;
    stmt->stmt = pStmt;
    consume(PT, SEMICOLON, "Expect ';' after value.");
    return stmt;
}

Stmt* expressionStatement(ParsedTokens* PT){
    Stmt* stmt = malloc(sizeof(Stmt));
    ExprStmt* eStmt = malloc(sizeof(ExprStmt));
    eStmt->expr = expression(PT);
    stmt->type = STMT_EXPR;
    stmt->stmt = eStmt;
    consume(PT, SEMICOLON, "Expect ';' after value.");
    return stmt;
}

static Stmt* functionStatment(ParsedTokens* PT){
    Token name = consume(PT, IDENTIFIER, "Except function name");
    FunctionStmt* fStmt = malloc(sizeof(FunctionStmt));
    Token* params = NULL;
    Stmt* body = NULL;
    TokenType types[] = {COMMA};
    int paramLen = 0;

    consume(PT, LEFT_PAREN, "Expect '(' after function name.");
    if(!MATCH(PT->tokens[PT->current].type, RIGHT_PAREN)){
        do {
            if(paramLen >= 16){
                error(PT, "Cannot have more than 16 arguments");
            }
            params = realloc(params, sizeof(Token) * (paramLen + 1));
            params[paramLen] = consume(PT, IDENTIFIER, "Expect parameter name");
            paramLen++;
        }while(match(PT, PT->tokens[PT->current].type, types, 1));

    }

    consume(PT, RIGHT_PAREN, "Expect ')' after parameters.");
    consume(PT, LEFT_BRACE, "Expect '{' before funcion body.");

    body = blockStatment(PT);

    fStmt->body = (BlockStmt*)body->stmt;
    fStmt->params = params;
    fStmt->name = name;
    fStmt->paramLen = paramLen;

    return newStatement(STMT_FUNCTION, fStmt);
}

Stmt* varStatement(Expr* initializer, Token name){
    Stmt* stmt = malloc(sizeof(Stmt));
    VarStmt* vStmt = malloc(sizeof(VarStmt));
    vStmt->expr = initializer;
    vStmt->name = name;
    stmt->type = STMT_VAR;
    stmt->stmt = vStmt;
    return stmt;
}
Stmt* blockStatment(ParsedTokens* PT){
    Stmt* stmt = malloc(sizeof(Stmt));
    BlockStmt* bStmt = malloc(sizeof(VarStmt));
    bStmt->statements = NULL;
    bStmt->stmtLen = 0;

    while(!MATCH(PT->tokens[PT->current].type, RIGHT_BRACE)){
        bStmt->statements = realloc(bStmt->statements, sizeof(Stmt) * (bStmt->stmtLen + 1));
        bStmt->statements[bStmt->stmtLen] = declaration(PT);
        bStmt->stmtLen++;
    }

    stmt->type = STMT_BLOCK;
    stmt->stmt = bStmt;
    consume(PT, RIGHT_BRACE, "Expect '}' after block.");
    
    return stmt;
}

Stmt* ifStatment(ParsedTokens* PT){
    Stmt* stmt = malloc(sizeof(Stmt));
    IfStmt* iStmt = malloc(sizeof(IfStmt));
    consume(PT, LEFT_PAREN, "Expect '(' after 'if'.");
    Expr* condition = expression(PT);
    consume(PT, RIGHT_PAREN, "Expect ')' after if condition");

    Stmt* thenBranch = statement(PT);
    Stmt* elseBranch = NULL;
    if(MATCH(PT->tokens[PT->current].type, ELSE)){
        PT->current++;
        elseBranch = realloc(elseBranch, sizeof(Stmt));
        elseBranch = statement(PT);
    }

    iStmt->condition = condition;
    iStmt->thenBranch = thenBranch;
    iStmt->elseBranch = elseBranch;

    stmt->type = STMT_IF;
    stmt->stmt = iStmt;

    return stmt;
}

Stmt* whileStatment(ParsedTokens* PT){
    Stmt* stmt = malloc(sizeof(Stmt));
    WhileStmt* wStmt = malloc(sizeof(WhileStmt));
    
    consume(PT, LEFT_PAREN, "Expect '(' after 'while'.");
    Expr* condition = expression(PT);
    consume(PT, RIGHT_PAREN, "Expect ')' after condition.");
    
    Stmt* body = statement(PT);

    wStmt->body = body;
    wStmt->condition = condition;

    stmt->type = STMT_WHILE;
    stmt->stmt = wStmt;

    return stmt;
}

static Stmt* forStatment(ParsedTokens* PT){
    Stmt* body = NULL;
    Stmt* initializer = NULL;
    Expr* increment = NULL;
    Expr* condition = NULL;
    
    BlockStmt* wrappedBody = NULL;
    BlockStmt* wrappedForAndInit = NULL;

    WhileStmt* wrapperFor = NULL;
    
    ExprStmt* wrappedStep = NULL; 
    

    consume(PT, LEFT_PAREN, "Expect '(' after 'for'.");

    if(MATCH(PT->tokens[PT->current].type, SEMICOLON)){
        PT->current++;        
        initializer = NULL;
    }else if(MATCH(PT->tokens[PT->current].type, VAR)){
        PT->current++;    
        initializer = varDeclaration(PT);
    }else{ 
        initializer = expressionStatement(PT);
    }
    
    if(!MATCH(PT->tokens[PT->current].type, SEMICOLON)){
        condition = expression(PT);
    }
    consume(PT, SEMICOLON, "Expect ';' after loop condition.");

    if(!MATCH(PT->tokens[PT->current].type, RIGHT_PAREN)){
        increment = expression(PT);
    }
    consume(PT, RIGHT_PAREN, "Expect ')' after for clauses.");

    body = statement(PT);
    if(increment != NULL){
        wrappedBody = malloc(sizeof(BlockStmt));
        wrappedBody->statements = malloc(sizeof(Stmt) * 2);
        wrappedStep = malloc(sizeof(ExprStmt));
        wrappedStep->expr = increment;

        wrappedBody->statements[0] = body;
        wrappedBody->statements[1] = newStatement(STMT_EXPR, wrappedStep);
        wrappedBody->stmtLen = 2;
    
        body = newStatement(STMT_BLOCK, wrappedBody);
    }

    if(condition == NULL){
        condition = newStatement(EXPR_LITERAL, newLiteral(LITERAL_BOOL, 1));
    }

    wrapperFor = malloc(sizeof(WhileStmt));
    wrapperFor->condition = condition;
    wrapperFor->body = body;
    body = newStatement(STMT_WHILE, wrapperFor);

    if(initializer != NULL){
        wrappedForAndInit = malloc(sizeof(BlockStmt));
        wrappedForAndInit->statements = malloc(sizeof(Stmt) * 2);
        wrappedForAndInit->statements[0] = initializer;
        wrappedForAndInit->statements[1] = body;
        wrappedForAndInit->stmtLen = 2;

        body = newStatement(STMT_BLOCK, wrappedForAndInit);
    }

    return body;
}

Stmt* statement(ParsedTokens* PT) {
    Token token = PT->tokens[PT->current];
    if(MATCH(token.type, FOR)){
        PT->current++;
        return forStatment(PT);
    }
    if(MATCH(token.type, WHILE)){
        PT->current++;
        return whileStatment(PT);
    }
    if(MATCH(token.type, IF)){
        PT->current++;
        return ifStatment(PT);
    }
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

Stmt* varDeclaration(ParsedTokens* PT) {
    Token name = consume(PT, IDENTIFIER, "Expected variable name.");
    
    Expr* initializer = NULL;
    TokenType types[] = { EQUAL };
    if(match(PT, PT->tokens[PT->current].type, types, 1)){
        initializer = expression(PT);
    }

    consume(PT, SEMICOLON, "Expect ';' after variable declaration.");
    
    return varStatement(initializer, name);
}

Stmt* declaration(ParsedTokens* PT) {
    Token token = PT->tokens[PT->current];
    if(MATCH(token.type, FUN)){
        PT->current++;

        return functionStatment(PT); 
    }
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
