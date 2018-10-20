#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fileio.h"
#include "token.h"


Token normalToken(TokenType type, char* lexeme, char* literal, int line){
    int length;
    Token token;

    if(lexeme == NULL){
        token.lexeme = NULL;
    }else{
        length = strlen(lexeme) + 1;
        token.lexeme = malloc(sizeof(char) * length);
        strncpy(token.lexeme, lexeme, length);
    }

    if(literal == NULL){
        token.literal = NULL;
    }else{
        length = strlen(literal) + 1;
        token.literal = malloc(sizeof(char) * length);
        strncpy(token.literal, literal, length);
    }

    token.line = line;
    token.type = type;

    return token;
}

Token simpleToken(TokenType type, char* lexeme, int line){
    return normalToken(type, lexeme, NULL, line);
}

Token idToken(char* lexeme, char* literal, int line){
    TokenType type;

    if (strcmp(literal, AND_KEY) == 0) {
        type = AND;
    } else if (strcmp(literal, CLASS_KEY) == 0) {
        type = CLASS;
    } else if (strcmp(literal, ELSE_KEY) == 0) {
        type = ELSE;
    } else if (strcmp(literal, FALSE_KEY) == 0) {
        type = FALSE;
    } else if (strcmp(literal, FUN_KEY) == 0) {
        type = FUN;
    } else if (strcmp(literal, FOR_KEY) == 0) {
        type = FOR;
    } else if (strcmp(literal, IF_KEY) == 0) {
        type = IF;
    } else if (strcmp(literal, NULL_KEY) == 0) {
        type = NULL_L;
    } else if (strcmp(literal, OR_KEY) == 0) {
        type = OR;
    } else if (strcmp(literal, PRINT_KEY) == 0) {
        type = PRINT;
    } else if (strcmp(literal, RETURN_KEY) == 0) {
        type = RETURN;
    } else if (strcmp(literal, SUPER_KEY) == 0) {
        type = SUPER;
    } else if (strcmp(literal, THIS_KEY) == 0) {
        type = THIS;
    } else if (strcmp(literal, TRUE_KEY) == 0) {
        type = TRUE;
    } else if (strcmp(literal, VAR_KEY) == 0) {
        type = VAR;
    } else if (strcmp(literal, WHILE_KEY) == 0) {
        type = WHILE;
    } else {
        type = IDENTIFIER;
    }

    return normalToken(type, lexeme, literal, line);
}

void addToken(Tokenizer* tokenizer, Token token){
    tokenizer->tokens = realloc(tokenizer->tokens, sizeof(Token) * (tokenizer->tokensLen + 1));

    tokenizer->tokens[tokenizer->tokensLen] = token;
    tokenizer->tokensLen++;
}

int matchNext(char* code, char match, int codeLength, int* current){
    if((*current) > codeLength){
        return 0;
    }

    if(code[(*current)] != match){
        return 0;
    }

    (*current)++;

    return 1;
}

char advance(char* code, int* current){
    return code[(*current)++];
}

int isDigit(char c){
    if (c >= '0' && c <= '9') return 1;
    return 0;
}

int isAlpha(char c){
    if((c >= 'a' && c <= 'z') ||      
           (c >= 'A' && c <= 'Z') ||      
            c == '_') return 1;
    return 0;
}

int isAlphaNumeric(char c){
    if(isAlpha(c) || isDigit(c)) return 1;
    return 0;
}

char* identifier(char* code, int* current){
    char* idString = NULL;
    int len = 1;
    int start = (*current) - 1;

    while(isAlphaNumeric(code[*current])){
        (*current)++;
        len++;
    }

    idString = realloc(idString, len);
    memcpy(idString, &(code[start]), len);
    idString[len] = '\0';

    return idString;
}

char* number(char* code, int* current){
    char* numString = NULL;
    int len = 0;
    --(*current);
    int start = (*current);
    while(isDigit(code[(*current)])){
        (*current)++;
        len++;
    }

    if(code[*current] == '.' && isDigit(code[(*current) + 1])){
        
        //consume the '.'
        (*current)++;

         while(isDigit(code[*current])){
            (*current)++;
            len++;
        }
        len++;

    }
    if(len > 1) len++;

    numString = realloc(numString, len);
    memcpy(numString, &(code[start]), len);
    numString[len] = '\0';

    return numString;
}

char* string(char* code, int* current, int* line){
    char* string = NULL;
    int len = 0;
    int start = *current;
    while(code[(*current)] != '"'){
        
        if(code[(*current)] == '\n'){
            (*line)++;
        }
        len++;
        (*current)++;
    }
    
    //after while loop add 1 to current so advance function skips the last " char
    (*current)++;
    string = realloc(string, len);
    memcpy(string, &(code[start]), len);
    string[len] = '\0';

    return string;
    
}

Tokenizer tokenizer(CodeInfo* ci){
    Token token;
    TokenType type;

    char* literal = NULL;
    char* codeChars = ci->chars;
    int codeLength = ci->codeLength;

    int current = 0;
    int line = 1;

    Tokenizer tokenizer;
    tokenizer.tokens = NULL;
    tokenizer.tokensLen = 0;
    

    while(!IS_AT_END(current, codeLength)){
        char c = advance(codeChars, &current);
        switch(c){
            case '(': 
                token = simpleToken(LEFT_PAREN, "(", line);
                addToken(&tokenizer, token);
                break;     
            case ')': 
                token = simpleToken(RIGHT_PAREN, ")",line);
                addToken(&tokenizer, token);
                break;    
            case '{': 
                token = simpleToken(LEFT_BRACE, "{",line);
                addToken(&tokenizer, token);                
                break;     
            case '}': 
                token = simpleToken(RIGHT_BRACE, "}",line); 
                addToken(&tokenizer, token);                
                break;    
            case ',': 
                token = simpleToken(COMMA, ",",line); 
                addToken(&tokenizer, token);                
                break;          
            case '.': 
                token = simpleToken(DOT, ".",line); 
                addToken(&tokenizer, token);                
                break;            
            case '-': 
                token = simpleToken(MINUS, "-",line); 
                addToken(&tokenizer, token);                
                break;          
            case '+': 
                token = simpleToken(PLUS, "+",line); 
                addToken(&tokenizer, token);                
                break;           
            case ';': 
                token = simpleToken(SEMICOLON, ";",line); 
                addToken(&tokenizer, token);                
                break;      
            case '*': 
                token = simpleToken(STAR, "*",line); 
                addToken(&tokenizer, token);                
                break;
            case '!':
                type = matchNext(codeChars, '=', codeLength, &current) ? BANG_EQUAL : BANG;
                token = simpleToken(type, "!", line);
                addToken(&tokenizer, token);                
                break;
            case '=':
                type = matchNext(codeChars, '=', codeLength, &current) ? EQUAL_EQUAL : EQUAL;
                token = simpleToken(type, "=", line);
                addToken(&tokenizer, token);                
                break;
            case '<':
                type = matchNext(codeChars, '=', codeLength, &current) ? LESS_EQUAL : LESS;
                token = simpleToken(type, "<", line);
                addToken(&tokenizer, token);                
                break;
            case '>':
                type = matchNext(codeChars, '=', codeLength, &current) ? GREATER_EQUAL : GREATER;
                token = simpleToken(type, ">", line);
                addToken(&tokenizer, token);                
                break;
            case '/':
                if(matchNext(codeChars, '/', codeLength, &current)){
                    while(current < codeLength && codeChars[current] != '\n'){
                        current++;
                    }
                }else if(matchNext(codeChars, '*', codeLength, &current)){
                    while(current < codeLength){
                        if(codeChars[current] == '*' && codeChars[current + 1] == '/'){
                            //add 2 to current to skip * and / chars.
                            //wihtout this, * and / will be counted as tokens and
                            //it breaks the program
                            current = current + 2;
                            break;
                        }
                        if(codeChars[current] == '\n') line++;
                        current++;
                    }
                }else{
                    token = simpleToken(SLASH, "/", line);
                    addToken(&tokenizer, token);
                }
                break;
            case '"':
                literal = string(codeChars, &current, &line);
                token = normalToken(STRING, "\"", literal, line);
                addToken(&tokenizer, token);
                break;
            case ' ':                                    
            case '\r':                                   
            case '\t':                                   
                // Ignore whitespace.                      
                break;

            case '\n':                                   
                line++;                                    
                break;

            
            default:
                if(isDigit(c)){
                    literal = number(codeChars, &current);
                    token = normalToken(NUMBER, literal, literal, line);
                    addToken(&tokenizer, token);
                }else if(isAlpha(c)){
                    literal = identifier(codeChars, &current);
                    token = idToken(literal, literal, line);
                    addToken(&tokenizer, token);
                }
                break;
        }
    }
    free(ci);
    return tokenizer;

}

