#ifndef _TOKENS_H_
#define _TOKENS_H_

#include <stdio.h>

typedef void (*TokenProcess)(FILE*, char*, int);

void processByte(FILE* execfile, char* subline, int tokenid);
void processIfElse(FILE* execfile, char* subline, int tokenid);
void processWhileLoop(FILE* execfile, char* subline, int tokenid);

#endif


