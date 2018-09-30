#ifndef _LOX_FILEIO_H_
#define _LOX_FILEIO_H_
#define _GNU_SOURCE


typedef struct CodeInfo{
	unsigned long codeLength;
	char* chars;	
}CodeInfo;

CodeInfo* readFile(char* filepath);

#endif
