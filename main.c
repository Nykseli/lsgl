#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fileio.h"
#include "token.h"

void runPrompt();
void runFile();
void run();
char* readLine(const char* prompt);

#define LINEBUFSIZE 1000

int main(int argc, char* argv[]){
    
    if(argc > 2){
        printf("Usage: clox [script]");
        //parse file
        return 64;
    }else if (argc == 2){
        runFile(argv[1]);
    }else{
        runPrompt();
    }
}



void runPrompt(){
    char* line = NULL;
    for (line = readLine("> "); line != NULL && strcmp(line, "exit()\n") != 0; line = readLine("> ")) {
                printf("%s", line);
                free(line);
}
}

char* readLine(const char* prompt){
    int size = sizeof(char) * LINEBUFSIZE;
    char* line = (char*)malloc(size);
    memset((void*)line, 0, size);
    printf("%s", prompt);
    fflush(stdin);
    fgets(line, size, stdin);
    return line;
}

void runFile(char* filePath){
    CodeInfo* codeInfo = readFile(filePath);
    printf("%s", codeInfo->chars);
    Tokenizer tokenzr = tokenizer(codeInfo);

    for(int i = 0;  i< tokenzr.tokensLen; i++){
        printf("lexeme: %s %d\n", tokenzr.tokens[i].lexeme, tokenzr.tokens[i].line);
        if(tokenzr.tokens[i].literal != NULL){
        printf("lexeme lit: %s\n", tokenzr.tokens[i].literal);
        printf("lexeme lit len: %d\n", strlen(tokenzr.tokens[i].literal));
        }
    }
    
}

void run(){

}
