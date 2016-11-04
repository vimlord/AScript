#ifndef _FUNCTIONCALL_H_
#define _FUNCTIONCALL_H_

#include <stdlib.h>

#include "srccompile.h"

/**
 * Constructs a stack frame that the program can then use to run executions.
 * Should push value and pointer parameters to the stack.
 *
 * execfile - The file currently subject to writing.
 * params   - A string containing a list of arguments.
 */
int buildStkFrame(FILE* execfile, char* params);


void performFunctionCall(FILE* execfile, char* params, CMP_TOK type);

/**
 * Returns from a function. Places the return value where it needs to be.
 * Called on the end that is returning.
 */
void returnFromFunction(FILE* execfile);

/**
 * Uses the remains of the stack frame to finalize return from a function.
 * Called on the end that called the function.
 *
 * execfile - The file to write to.
 * size     - The number of bytes in the file.
 */
void finalizeReturn(FILE* execfile, int size);

#endif

