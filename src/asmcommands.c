#include "asmcommands.h"

int comparisons = 0;

void writeAsmBlock(FILE* file, char* line) {
    int i = -1;
    while(line[++i]);
    
    //printf("%s", line);

    fwrite(line, 1, i, file);
}
void writeComment(FILE* file, char* comment) {
    writeAsmBlock(file, "; ");
    writeAsmBlock(file, comment);
    writeAsmBlock(file, "\n");
}

void branchEQ(FILE* execfile, int regA, int regB, char* br) {
    char label[64];
    sprintf(label, "comp%i", comparisons++);
    
    char scratch[64];
    //First, compare
    sprintf(scratch, "cp r%i, r%i\n", regA, regB);
    writeAsmBlock(execfile, scratch);
    //Then, branch if
    sprintf(scratch, "brne %s\n", label);
    writeAsmBlock(execfile, scratch);
    sprintf(scratch, "jmp %s\n%s:\n", br, label);
    writeAsmBlock(execfile, scratch);

}

void branchNE(FILE* execfile, int regA, int regB, char* br) {
    char scratch[64];
    //First, compare
    sprintf(scratch, "cpse r%i, r%i\n", regA, regB);
    writeAsmBlock(execfile, scratch);
    //Then, branch if
    sprintf(scratch, "jmp %s\n", br);
    writeAsmBlock(execfile, scratch);

}

void branchGE(FILE* execfile, int regA, int regB, char* br) {
    char scratch[64];
    //First, compare
    sprintf(scratch, "cp r%i, r%i\n", regA, regB);
    writeAsmBlock(execfile, scratch);
    //Then, branch if
    sprintf(scratch, "brge %s\n", br);
    writeAsmBlock(execfile, scratch);

}

void branchGT(FILE* execfile, int regA, int regB, char* br) {
    char scratch[64];
    //First, compare
    sprintf(scratch, "cp r%i, r%i\n", regB, regA);
    writeAsmBlock(execfile, scratch);
    //Then, branch if
    sprintf(scratch, "brlt %s\n", br);
    writeAsmBlock(execfile, scratch);

}


void branchLE(FILE* execfile, int regA, int regB, char* br) {
    char scratch[64];
    //First, compare
    sprintf(scratch, "cp r%i, r%i\n", regB, regA);
    writeAsmBlock(execfile, scratch);
    //Then, branch if
    sprintf(scratch, "brge %s\n", br);
    writeAsmBlock(execfile, scratch);

}

void branchLT(FILE* execfile, int regA, int regB, char* br) {
    char scratch[64];
    //First, compare
    sprintf(scratch, "cp r%i, r%i\n", regA, regB);
    writeAsmBlock(execfile, scratch);
    //Then, branch if
    sprintf(scratch, "brlt %s\n", br);
    writeAsmBlock(execfile, scratch);

}

void jumpToLabel(FILE* execfile, char* label) {
    char scratch[64];
    
    //Jump to label
    sprintf(scratch, "jmp %s\n", label);
    writeAsmBlock(execfile, scratch);

}

void copyReg(FILE* execfile, int dst, int src) {
    char cmdline[64];
    sprintf(cmdline, "mov r%i, r%i\n", dst, src);
    
    writeAsmBlock(execfile, cmdline);
}

void copyRegFromMem(FILE* execfile, int dst, int src) {
    int zh = src / 0x100, zl = src % 0x100;
    char scratch[64];

    sprintf(scratch, "ldi zh, $%x\n", zh);
    writeAsmBlock(execfile, scratch);
    
    sprintf(scratch, "ldi zl, $%x\n", zl);
    writeAsmBlock(execfile, scratch);
    
    sprintf(scratch, "ld r%i, z\n", dst);
    writeAsmBlock(execfile, scratch);
}

void copyRegToMem(FILE* execfile, int dst, int src) {
    char scratch[64];

    sprintf(scratch, "sts 0x%x, r%i\n", dst, src);
    writeAsmBlock(execfile, scratch);
}
void loadReg(FILE* execfile, int r, char* v) {
    char cmdline[64];
    sprintf(cmdline, "ldi r%i, %s\n", r, v);

    writeAsmBlock(execfile, cmdline);
}

void loadRegV(FILE* execfile, int r, int v) {
    char value[16];
    sprintf(value, "%i", v);
    loadReg(execfile, r, value);
}

void addReg(FILE* execfile, int a, int b) {
    char cmdline[64];
    sprintf(cmdline, "add r%i, r%i\n", a, b);
    
    writeAsmBlock(execfile, cmdline);
}

void subReg(FILE* execfile, int a, int b) {
    char cmdline[64];
    sprintf(cmdline, "sub r%i, r%i\n", a, b);
    
    writeAsmBlock(execfile, cmdline);
}

void mulRegs(FILE* execfile, int a, int b) {
    char cmdline[64];
    sprintf(cmdline, "mul r%i, r%i\n", a, b);

    writeAsmBlock(execfile, cmdline);
}

void andReg(FILE* execfile, int a, int b) {
    char cmdline[64];
    sprintf(cmdline, "and r%i, r%i\n", a, b);
    
    writeAsmBlock(execfile, cmdline);
}

void orReg(FILE* execfile, int a, int b) {
    char cmdline[64];
    sprintf(cmdline, "or r%i, r%i\n", a, b);

    writeAsmBlock(execfile, cmdline);
}

void xorReg(FILE* execfile, int a, int b) {
    char cmdline[64];
    sprintf(cmdline, "eor r%i, r%i\n", a, b);

    writeAsmBlock(execfile, cmdline);
}

void eqBool(FILE* execfile, int a, int b, int dst) {
    char label[64];
    sprintf(label, "comp%i", comparisons++);

    //Sets the initial value for if false, then compares
    loadReg(execfile, dst, "1"); 

    //Branch if false
    branchEQ(execfile, a, b, label);
    loadReg(execfile, dst, "0");
    writeAsmBlock(execfile, label);
    writeAsmBlock(execfile, ":\n");

}

void neBool(FILE* execfile, int a, int b, int dst) {
    char label[64];
    sprintf(label, "comp%i", comparisons++);

    //Sets the initial value for if false, then compares
    loadReg(execfile, dst, "1"); 

    //Branch if false
    branchNE(execfile, a, b, label);
    loadReg(execfile, dst, "0");
    writeAsmBlock(execfile, label);
    writeAsmBlock(execfile, ":\n");

}

void geBool(FILE* execfile, int a, int b, int dst) {
    char label[64];
    sprintf(label, "comp%i", comparisons++);

    //Sets the initial value for if false, then compares
    loadReg(execfile, dst, "$1"); 

    //Branch if false
    branchGE(execfile, a, b, label);
    loadReg(execfile, dst, "$0");
    writeAsmBlock(execfile, label);
    writeAsmBlock(execfile, ":\n");

}

void gtBool(FILE* execfile, int a, int b, int dst) {
    char label[64];
    sprintf(label, "comp%i", comparisons++);

    //Sets the initial value for if false, then compares
    loadReg(execfile, dst, "$1"); 

    //Branch if false
    branchGT(execfile, a, b, label);
    loadReg(execfile, dst, "$0");
    writeAsmBlock(execfile, label);
    writeAsmBlock(execfile, ":\n");

}

void leBool(FILE* execfile, int a, int b, int dst) {
    char label[64];
    sprintf(label, "comp%i", comparisons++);

    //Sets the initial value for if false, then compares
    loadReg(execfile, dst, "$1"); 

    //Branch if false
    branchLE(execfile, a, b, label);
    loadReg(execfile, dst, "$0");
    writeAsmBlock(execfile, label);
    writeAsmBlock(execfile, ":\n");

}

void ltBool(FILE* execfile, int a, int b, int dst) {
    char label[64];
    sprintf(label, "comp%i", comparisons++);

    //Sets the initial value for if false, then compares
    loadReg(execfile, dst, "$1"); 

    //Branch if false
    branchLT(execfile, a, b, label);
    loadReg(execfile, dst, "$0");
    writeAsmBlock(execfile, label);
    writeAsmBlock(execfile, ":\n");

}

void stackPush(FILE* execfile, int reg) {
    char scratch[64];

    sprintf(scratch, "push r%i\n", reg);
    writeAsmBlock(execfile, scratch);
    
}

void stackPop(FILE* execfile, int reg) {
    char scratch[64];

    sprintf(scratch, "pop r%i\n", reg);
    writeAsmBlock(execfile, scratch);

}


