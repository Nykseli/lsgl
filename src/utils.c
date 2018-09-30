#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

void die(const char *s){
	
	printf("%s\n", s);
    perror(s);
    exit(1);
}
