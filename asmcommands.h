#include <stdio.h>

void writeAsmBlock(FILE* file, char* line);

void copyReg(FILE* execfile, int dst, int src);
void copyRegFromMem(FILE* execfile, int dst, int src);
void copyRegToMem(FILE* execfile, int dst, int src);
void loadReg(FILE* execfile, int r, char* v);

//Adds b into a.
void addReg(FILE* execfile, int a, int b);
void subReg(FILE* execfile, int a, int b);

//Multiplies two registers. The contents are in r0 and r1
void mulRegs(FILE* execfile, int a, int b);


