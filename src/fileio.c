#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "fileio.h"
#include "utils.h"


CodeInfo* readFile(char* filepath) {
    CodeInfo* CI = malloc(sizeof(CodeInfo));

    int length = 0;
    FILE* fp = fopen(filepath, "r");
    char* buf = NULL;

    if (fp == NULL) {
        //except("Cannot open file\n");
        return NULL;
    }

    if (!fseek(fp, SEEK_SET, SEEK_END)) {
        length = ftell(fp);
        if (!length) {
            //except("Exceeded Maximum File Size\n");
            return NULL;
        }

        rewind(fp);
    }
    length++;
    buf = (char*)malloc(length + 1);
    memset(buf, 0, length);
    fread(buf, 1, length, fp);
    buf[length] = '\0';
    if (!fclose(fp)) {
        CI->chars = malloc(length);
        memcpy(CI->chars, buf, length);
        CI->codeLength = length;
        free(buf);
        return CI;
    }
    free(buf);
    return NULL;


}
