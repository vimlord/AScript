#ifndef _PEMDAS_H_
#define _PEMDAS_H_

#include "mathconvert.h"

typedef void (*MathOperation)(FILE*, char*, char*, int);

//Standard PEMDAS operations
void addOperation(FILE* execfile, char* partA, char* partB, int dst);
void subOperation(FILE* execfile, char* partA, char* partB, int dst);
void mulOperation(FILE* execfile, char* partA, char* partB, int dst);

//Bitwise boolean expressions.
void bitAndOperation(FILE* execfile, char* partA, char* partB, int dst);
void bitOrOperation(FILE* execfile, char* partA, char* partB, int dst);


#endif

