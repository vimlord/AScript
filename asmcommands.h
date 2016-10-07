#include <stdio.h>

void writeAsmBlock(FILE* file, char* line);
void writeComment(FILE* file, char* comment);

void branchEQ(FILE* execfile, int regA, int regB, char* br);
void branchNE(FILE* execfile, int regA, int regB, char* br);
void branchGE(FILE* execfile, int regA, int regB, char* br);
void branchGT(FILE* execfile, int regA, int regB, char* br);
void branchLE(FILE* execfile, int regA, int regB, char* br);
void branchLT(FILE* execfile, int regA, int regB, char* br);

void jumpToLabel(FILE* execfile, char* label);

void copyReg(FILE* execfile, int dst, int src);
void copyRegFromMem(FILE* execfile, int dst, int src);
void copyRegToMem(FILE* execfile, int dst, int src);
void loadReg(FILE* execfile, int r, char* v);

void addReg(FILE* execfile, int a, int b);
void subReg(FILE* execfile, int a, int b);

void andReg(FILE* execfile, int a, int b);
void orReg(FILE* execfile, int a, int b);
void xorReg(FILE* execfile, int a, int b);

void eqReg(FILE* execfile, int a, int b, int dst);
void neReg(FILE* execfile, int a, int b, int dst);
void geReg(FILE* execfile, int a, int b, int dst);
void gtReg(FILE* execfile, int a, int b, int dst);
void leReg(FILE* execfile, int a, int b, int dst);
void ltReg(FILE* execfile, int a, int b, int dst);

//Multiplies two registers. The contents are in r0 and r1
void mulRegs(FILE* execfile, int a, int b);


