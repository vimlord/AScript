#include "list.h"

#include <stdio.h>

#ifndef _SRCCMP_H_
#define _SRCCMP_H_

//Tokens
typedef char* CMP_TOK;

struct var_frame {
    char* name;
    char* type;
    int addr;
};

typedef struct var_frame* VarFrame;

List getVars();

int getLoopDepth();

int sizeOfType(char* type);
char* variableTypeOf(char* var);
int variableSizeOf(char* var);
int stackAddressOfVar(char* var);
void loadStackAddressOf(FILE* execfile, char* var); //Loads the address into y

int compTok(CMP_TOK a, CMP_TOK b);
void setCompilerStackTop(int idx);

/**
 * Adds a variable to the stack.
 *
 * execfile - The file containing the instructions.
 * type     - The type of variable being added.
 * val      - The initial value.
 * varname  - The name of the variable.
 *
 * Returns the address of the variable in the stack frame.
 */
int addVariable(FILE* stkfile, CMP_TOK type, char* varname, int nbytes);

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

void processToken(FILE* execfile, int tokidx, char* subline);


#endif
