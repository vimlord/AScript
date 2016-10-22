#ifndef _OPTIMIZATION_H_
#define _OPTIMIZATION_H_

#include <stdio.h>

/**
 * Performs optimizations on assembly code.
 *
 * input  - A .asm file containing the initial assembly code.
 * output - The file that will hold the results.
 */
int optimizeAsm(FILE* src, FILE* dst);

void performOptimizations(FILE* src, FILE* dst);

#endif

