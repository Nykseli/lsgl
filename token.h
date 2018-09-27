#ifndef _LOX_TOKEN_H_
#define _LOX_TOKEN_H_

typedef enum tokentype {
    LEFT_PAREN,
    RIGHT_PAREN,
    LEFT_BRACE,
    RIGHT_BRACE,
    COMMA,
    DOT,
    MINUS,
    PLUS,
    SEMICOLON,
    SLASH,
    STAR,

    BANG,
    BANG_EQUAL,
    EQUAL,
    EQUAL_EQUAL,
    GREATER,
    GREATER_EQUAL,
    LESS,
    LESS_EQUAL,

    IDENTIFIER,
    STRING,
    NUMBER,

    AND,
    CLASS,
    ELSE,
    FALSE,
    FUN,
    FOR,
    IF,
    NIL,
    OR,
    PRINT,
    RETURN,
    SUPER,
    THIS,
    TRUE,
    VAR,
    WHILE,

    ENDOFFILE
} TokenType;

typedef struct token {
    TokenType type;
    char* literal;
    char* lexeme;
    int column;
    int line;
} Token;

typedef struct tokenizer {
    Token* tokens;
    int tokensLen;
} Tokenizer;

void addToken(Tokenizer* tokenizer, Token token);
int matchNext(char* code, char match, int codeLength, int* current);
char advance(char* code, int* current);
char* string(char* code, int* current, int* line);
int isDigit(char c);
char* number(char* code, int* current);
int isAlpha(char c);
int isAlphaNumeric(char c);

char* identifier(char* code, int* current);

Token token(TokenType type, char* lexeme, char* literal, int line);
Token simpleToken(TokenType type, char* lexeme, int line);
Token normalToken(TokenType type, char* lexeme, char* literal, int line);
Tokenizer tokenizer(CodeInfo* fi);

#define IS_AT_END(x, codeLength) ((x) >= (codeLength))

#define AND_KEY "and"
#define CLASS_KEY "class"
#define ELSE_KEY "else"
#define FALSE_KEY "false"
#define FUN_KEY "fun"
#define FOR_KEY "for"
#define IF_KEY "if"
#define NIL_KEY "nil"
#define OR_KEY "or"
#define PRINT_KEY "print"
#define RETURN_KEY "return"
#define SUPER_KEY "super"
#define THIS_KEY "this"
#define TRUE_KEY "true"
#define VAR_KEY "var"
#define WHILE_KEY "while"

#endif
