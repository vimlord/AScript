#include "list.h"

#include <stdio.h>

#ifndef _SRCCMP_H_
#define _SRCCMP_H_

//Tokens
typedef char* CMP_TOK;

char* TOKENS[4]; 

List getVars();

int compTok(CMP_TOK a, CMP_TOK b);

/**
 * Adds the data to the stack frame file that correlates 
 * with the given value.
 *
 * Returns the variable number.
 */
int addVariable(FILE* stkfile, CMP_TOK type, char* varname);

/**
 * Parses a code snippet into assembly code.
 *
 * execfile - The file to which the code will be written.
 * segment  - The segment that will be parsed.
 */

void parseSegment(FILE* execfile, char* code);

/**
 * Interprets a line of code and determines what
 * it should do. Makes the necessary changes to
 * the stack frame.
 */
void parseLine(FILE* execfile, char* line);

void processToken(FILE* execfile, CMP_TOK tok, char* subline);


#endif
