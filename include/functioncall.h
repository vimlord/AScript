#ifndef _FUNCTIONCALL_H_
#define _FUNCTIONCALL_H_

#include <stdlib.h>

#include "srccompile.h"

/**
 * Constructs a stack frame that the program can then use to run executions.
 * Should push value and pointer parameters to the stack.
 * Stores the original x on the stack, and replaces it with the new stack pointer.
 *
 * execfile - The file currently subject to writing.
 * params   - A string containing a list of arguments.
 * type     - The return type to prepare for.
 */
int buildStkFrame(FILE* execfile, char* params, CMP_TOK type);

/**
 * Performs a function call.
 *
 * execfile - The file to write the instructions to.
 * params   - The list of arguments given to the function.
 * type     - The function's return type, or NULL if there is no return type.
 */
void performFunctionCall(FILE* execfile, char* params, CMP_TOK type, char* name);

/**
 * Returns from a function.
 * Called on the end that is returning.
 */
void performFunctionReturn(FILE* execfile);

/**
 * Uses the remains of the stack frame to finalize return from a function.
 * Should be executed on the end that called the function.
 * Restores the value of x. If there is a return type, it will be
 * at the top of the stack. Should essentially undo buildStkFrame().
 *
 * execfile - The file to write to.
 * size     - The number of bytes in the file.
 * type     - The variable type, or NULL if no return value
 */
void finalizeReturn(FILE* execfile, int size, CMP_TOK type);

#endif

