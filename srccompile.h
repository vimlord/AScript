#include "list.h"

#include <stdio.h>

#ifndef _SRCCMP_H_
#define _SRCCMP_H_

//Tokens
typedef char* CMP_TOK;

char* TOKENS[2]; 

List getVars();

int compTok(CMP_TOK a, CMP_TOK b);

/**
 * Adds the data to the stack frame file that correlates 
 * with the given value.
 *
 * Returns the variable number.
 */
int addStackFrameVar(FILE* stkfile, CMP_TOK type, int val, char* varname);

/**
 * Interprets a line of code and determines what
 * it should do. Makes the necessary changes to
 * the stack frame.
 */
void parseLine(FILE* stkfile, FILE* execfile, char* line);

void processToken(FILE* stkfile, FILE* execfile, CMP_TOK tok, char* subline);


#endif
