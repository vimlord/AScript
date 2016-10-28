#ifndef _PEMDAS_H_
#define _PEMDAS_H_

#include "mathconvert.h"

typedef void (*MathOperation)(FILE*, char*, char*, int n);

//Standard PEMDAS operations
void addOperation(FILE* execfile, char* partA, char* partB, int n);
void subOperation(FILE* execfile, char* partA, char* partB, int n);
void mulOperation(FILE* execfile, char* partA, char* partB, int n);

//Bitwise boolean expressions.
void bitAndOperation(FILE* execfile, char* partA, char* partB, int n);
void bitOrOperation(FILE* execfile, char* partA, char* partB, int n);
void bitXorOperation(FILE* execfile, char* partA, char* partB, int n);

//Comparison
void boolEqOperation(FILE* execfile, char* partA, char* partB, int n);
void boolNeOperation(FILE* execfile, char* partA, char* partB, int n);
void boolGeOperation(FILE* execfile, char* partA, char* partB, int n);
void boolGtOperation(FILE* execfile, char* partA, char* partB, int n);
void boolLeOperation(FILE* execfile, char* partA, char* partB, int n);
void boolLtOperation(FILE* execfile, char* partA, char* partB, int n);

#endif

