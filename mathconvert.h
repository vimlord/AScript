#include <stdio.h>

int indexOfClosingChar(char* str, char start, char end);
char* closureContent(char* str, char start, char end);

char* parenthesesContent(char* start);
char* bracketContent(char* start);

char* contentToOperator(char* start, char op, char up, char down);

/**
 * Performs a PEMDAS operation based on an input.
 * calc - The calculation
 * dst  - The address where the result will be stored.
 */
void pemdas(FILE* execfile, char* calc, int dst);
