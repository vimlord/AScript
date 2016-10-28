#include <stdio.h>

/**
 * Writes the assembly code that performs a PEMDAS operation based
 * on the C-style order of operations.
 *
 * execfile - The destination for the code to be written to.
 * calc     - A segment of code that contains the formula, which should have closure.
 * dst      - The address in memory where the result will be stored.
 */
void pemdas(FILE* execfile, char* calc, int nbytes);

/**
 * Jumps to a label if the value of the conditional is true.
 *
 * cond  - A pemdas parseable statement.
 * label - The label that will be jumped to on meeting the condition.
 * ptr   - The address where the result of computing cond can safely be stored.
 */
void jumpIfTrue(FILE* execfile, char* cond, char* label, int nbytes);

/**
 * Jumps to a label if the value of the conditional is false.
 *
 * cond  - A pemdas parseable statement.
 * label - The label that will be jumped to on meeting the condition.
 * ptr   - The address where the result of computing cond can safely be stored.
 */
void jumpIfFalse(FILE* execfile, char* cond, char* label, int nbytes);
