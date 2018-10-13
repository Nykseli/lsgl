#ifndef _LOX_EXPR_H_
#define _LOX_EXPR_H_
#include "token.h"

typedef enum expr_type {
    EXPR_BINARY = 1,
    EXPR_GROUPING,
    EXPR_LITERAL,
    EXPR_UNARY,
    EXPR_VARIABLE,
    EXPR_ASSIGNMENT,
    EXPR_LOGICAL,
    EXPR_CALL,
    EXPR_GET,
    EXPR_SET,
    EXPR_THIS,
    EXPR_SUPER
} ExpressionType;

typedef enum literal_expr_type {
    LITERAL_NULL,
    LITERAL_BOOL,
    LITERAL_NUMBER,
    LITERAL_STRING
} LiteralType;


typedef struct expression {
    ExpressionType type;
    void* expr;
    unsigned int order;
} Expr;

typedef struct binary {
    //op stansd for operator because operator is reserved cpp word
    Token op;
    Expr* left;
    Expr* right;
} BinaryExpr;

typedef struct grouping {
    Expr* expr;
} GroupingExpr;

typedef struct literal {
    void* value;
    LiteralType type;
    int valueSize;
} LiteralExpr;

typedef struct unary {
    //op stansd for operator because operator is reserved cpp word
    Token op;
    Expr* right;
} UnaryExpr;

typedef struct variable {
    Token name;
}VarExpr;

typedef struct assignment {
    Token name;
    Expr* value;
}AssignExpr;

typedef enum stmt_type {
    STMT_EXPR,
    STMT_PRINT,
    STMT_VAR
} StmtType;

typedef struct stmt {
    StmtType type;
    Token name;
    Expr* expr;
} Stmt;

typedef struct stmt_parsed {
    Stmt* stmt;
    int stmtLen;
} ParsedStmt;

#endif
