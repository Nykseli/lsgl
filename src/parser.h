#ifndef _LSGL_PARSER_H_
#define _LSGL_PARSER_H_
#include "expr.h"
#include "token.h"

typedef struct parsedTokens {
    int current;
    int tokensSize;
    Token* tokens;
} ParsedTokens;



ParsedStmt parseTokens(Tokenizer tokenizer);

#define MATCH(x, type) ((x) == type)

#endif
