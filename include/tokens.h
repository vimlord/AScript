#ifndef _TOKENS_H_
#define _TOKENS_H_

#include <stdio.h>

typedef void (*TokenProcess)(FILE*, char*, int);

/**
 * Handles the 'byte' keyword. This is used to instantiate a byte.
 */
void processByte(FILE* execfile, char* subline, int tokenid);

/**
 * Handles the 'if' and 'else' keywords. Used for if-else.
 */
void processIfElse(FILE* execfile, char* subline, int tokenid);

/**
 * Handles the 'while' keyword. Used for while loops.
 */
void processWhileLoop(FILE* execfile, char* subline, int tokenid);

#endif


