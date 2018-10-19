#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

void die(const char *s){
	
	printf("%s\n", s);
    perror(s);
    exit(1);
}

void except(const char* format, ...){
    va_list list;
    va_start(list, format);
    vfprintf(stderr, format, list);
    va_end(list);
    exit(1);
}
