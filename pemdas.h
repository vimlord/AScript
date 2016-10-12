#ifndef _PEMDAS_H_
#define _PEMDAS_H_

#include "mathconvert.h"

typedef void (*MathOperation)(FILE*, char*, char*);

//Standard PEMDAS operations
void addOperation(FILE* execfile, char* partA, char* partB);
void subOperation(FILE* execfile, char* partA, char* partB);
void mulOperation(FILE* execfile, char* partA, char* partB);

//Bitwise boolean expressions.
void bitAndOperation(FILE* execfile, char* partA, char* partB);
void bitOrOperation(FILE* execfile, char* partA, char* partB);
void bitXorOperation(FILE* execfile, char* partA, char* partB);

//Comparison
void boolEqOperation(FILE* execfile, char* partA, char* partB);
void boolNeOperation(FILE* execfile, char* partA, char* partB);
void boolGeOperation(FILE* execfile, char* partA, char* partB);
void boolGtOperation(FILE* execfile, char* partA, char* partB);
void boolLeOperation(FILE* execfile, char* partA, char* partB);
void boolLtOperation(FILE* execfile, char* partA, char* partB);

#endif

