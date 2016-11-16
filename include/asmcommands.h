
#include <errno.h>
#include <stdio.h>

void writeAsmBlock(FILE* file, char* line);
void writeComment(FILE* file, char* comment);

/**
 * Computes truth statements and branches if true.
 *
 * regA - The left hand register for comparison.
 * regB - The right hand register for comparison.
 * br   - The label to branch to if true.
 */
void branchEQ(FILE* execfile, int regA, int regB, char* br);
void branchNE(FILE* execfile, int regA, int regB, char* br);
void branchGE(FILE* execfile, int regA, int regB, char* br);
void branchGT(FILE* execfile, int regA, int regB, char* br);
void branchLE(FILE* execfile, int regA, int regB, char* br);
void branchLT(FILE* execfile, int regA, int regB, char* br);

void jumpToLabel(FILE* execfile, char* label);

/**
 * Copies the components of a register into another.
 */
void copyReg(FILE* execfile, int dst, int src);
void copyRegFromMem(FILE* execfile, int dst, int src);
void copyRegToMem(FILE* execfile, int dst, int src);
void loadReg(FILE* execfile, int r, char* v);
void loadRegV(FILE* execfile, int r, int v);

void addReg(FILE* execfile, int a, int b);
void subReg(FILE* execfile, int a, int b);

//Multiplies two registers. The contents are in r0 and r1
void mulRegs(FILE* execfile, int a, int b);

void andReg(FILE* execfile, int a, int b);
void orReg(FILE* execfile, int a, int b);
void xorReg(FILE* execfile, int a, int b);

void eqBool(FILE* execfile, int a, int b, int dst);
void neBool(FILE* execfile, int a, int b, int dst);
void geBool(FILE* execfile, int a, int b, int dst);
void gtBool(FILE* execfile, int a, int b, int dst);
void leBool(FILE* execfile, int a, int b, int dst);
void ltBool(FILE* execfile, int a, int b, int dst);

//Stack push and pop
void stackPush(FILE* execfile, int reg);
void stackPop(FILE* execfile, int reg);


