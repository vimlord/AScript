#include "functioncall.h"

#include "asmcommands.h"
#include "mathconvert.h"
#include "strmanip.h"

#include <string.h>

int buildStkFrame(FILE* execfile, char* params, CMP_TOK type) {
    int size = 0;
    
    //Places the return value
    if(type) {
        writeAsmBlock(execfile, "ldi r16, 0\n");
        
        int i = -1, size = sizeOfType(type);
        while(++i < size)
            writeAsmBlock(execfile, "push r16\n");
        size = 0;
    }

    char* str = params;
    while(*str) {
        if(*str == ' ') {
            str = &str[1];
            continue;
        }
        
        //Finds and adds the next parameter.
        char* val = contentToOperator(str, ',', '(', ')');
        pemdas(execfile, val, 2);
        
        size += 2;
    }
    
    //Finalize the stack frame by pushing x_old and then storing sp in x.
    writeAsmBlock(execfile, "push xh\npush xl\nin xh, sph\nin xl, spl\n");

    return size;

}

void performFunctionCall(FILE* execfile, char* params, CMP_TOK type, char* name) {
    
    //First, the stack frame needs to be setup.
    int size = buildStkFrame(execfile, params, type);
    
    //Jumps to the function. The program counter will be pushed to the stack.
    char buff[256];
    sprintf(buff, "call function_%s\n", name);
    writeAsmBlock(execfile, buff);
    
    //Performs the final instructions that grab the return value and restore the stack
    finalizeReturn(execfile, size, type);

}

void finalizeReturn(FILE* execfile, int size, CMP_TOK type) {
    //At this point, the stack pointer should point to the high bit of x_old on the stack.
     
    //Restore x
    writeAsmBlock(execfile, "pop xl\npop xh\n");
    
    //Pops the values from the stack.
    int i = 0;
    while(i < size) {
        stackPop(execfile, 16);
        i++;
    }

    //All that should be left is the return value, which should be at the top of the stack.

}


