#include "asmcommands.h"

void writeAsmBlock(FILE* file, char* line) {
    int i = -1;
    while(line[++i]);

    fwrite(line, 1, i, file);
}

void copyReg(FILE* execfile, int dst, int src) {
    char cmdline[64];
    sprintf(cmdline, "mov $%x, $%x\n", dst, src);
    
    writeAsmBlock(execfile, cmdline);
}

void copyRegFromMem(FILE* execfile, int dst, int src) {
    int zh = src / 0x100, zl = src % 0x100;
    char scratch[64];

    sprintf(scratch, "ldi zh, $%x\n", zh);
    writeAsmBlock(execfile, scratch);
    
    sprintf(scratch, "ldi zl, $%x\n", zl);
    writeAsmBlock(execfile, scratch);
    
    sprintf(scratch, "ld 0x%x, z\n", dst);
    writeAsmBlock(execfile, scratch);
}

void copyRegToMem(FILE* execfile, int dst, int src) {
    char scratch[64];

    sprintf(scratch, "sts 0x%x, 0x%x\n", dst, src);
    writeAsmBlock(execfile, scratch);
}
void loadReg(FILE* execfile, int r, char* v) {
    char cmdline[64];
    sprintf(cmdline, "ldi 0x%x, %s\n", r, v);

    writeAsmBlock(execfile, cmdline);
}

void addReg(FILE* execfile, int a, int b) {
    char cmdline[64];
    sprintf(cmdline, "add 0x%x, 0x%x\n", a, b);
    
    writeAsmBlock(execfile, cmdline);
}

void subReg(FILE* execfile, int a, int b) {
    char cmdline[64];
    sprintf(cmdline, "sub 0x%x, 0x%x\n", a, b);
    
    writeAsmBlock(execfile, cmdline);
}

void mulRegs(FILE* execfile, int a, int b) {
    char cmdline[64];
    sprintf(cmdline, "sub 0x%x, 0x%x\n", a, b);

    writeAsmBlock(execfile, cmdline);
}


