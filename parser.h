#ifndef _LOX_PARSER_H_
#define _LOX_PARSER_H_
#include "expr.h"
#include "token.h"

typedef struct parsedTokens {
    int current;
    int tokensSize;
    Token* tokens;
} ParsedTokens;



Expr* parseTokens(Tokenizer tokenizer);

#define MATCH(x, type) ((x) == type)

#endif
