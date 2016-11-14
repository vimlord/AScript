#include "functioncall.h"

#include "asmcommands.h"
#include "mathconvert.h"
#include "strmanip.h"

#include <string.h>

int buildStkFrame(FILE* execfile, char* params, CMP_TOK type) {
    int size = 0;
    
    writeComment(execfile, "Building stack frame for function call");

    //Places the return value
    if(type) {
        writeAsmBlock(execfile, "ldi r16, 0\n");
        
        int i = -1, size = sizeOfType(type);
        while(++i < size)
            writeAsmBlock(execfile, "push r16\n");

        //The size of the stack should not include the return value.
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

        char buff[32 + strlen(val)];
        sprintf(buff, "Calculating parameter %s", val);
        writeComment(execfile, buff);

        pemdas(execfile, val, 2);

        str = &str[strlen(val)];
        if(*str) str = &str[1];

        size += 2;

        free(val);
    }

    //Finalize the stack frame by pushing x_old and then storing sp in x.
    writeComment(execfile, "Saves old stack pointer for later");
    writeAsmBlock(execfile, "push xh\npush xl\nin xh, sph\nin xl, spl\n");

    return size;

}

void performFunctionCall(FILE* execfile, char* params, CMP_TOK type, char* name) {
    
    char buff[256];

    sprintf(buff, "Calling function %s %s(%s)", type, name, params);
    writeComment(execfile, buff);


    //First, the stack frame needs to be setup.
    int size = buildStkFrame(execfile, params, type);
    
    //Jumps to the function. The program counter will be pushed to the stack.
    sprintf(buff, "call function_%s\n", name);
    writeAsmBlock(execfile, buff);
    
    //Performs the final instructions that grab the return value and restore the stack
    finalizeReturn(execfile, size, type);

}

void finalizeReturn(FILE* execfile, int size, CMP_TOK type) {
    //At this point, the stack pointer should point to the high bit of x_old on the stack.
     
    //Restore x
    writeComment(execfile, "Restoring old x");
    writeAsmBlock(execfile, "pop xl\npop xh\n");
    
    //Pops the values from the stack.
    writeAsmBlock(execfile, "in r16, spl\nin r17, sph\n");
    
    //Loads the size of the stack into memory.
    char buff[64];
    sprintf(buff, "ldi r18, %i\nldi r19, %i\n", size%256, size/256);
    writeAsmBlock(execfile, buff);

    //Subtract the stack size, and then drop the part of the frame.
    writeAsmBlock(execfile, "sub r16, r18\nsbc r17, r19\nout spl, r16\nout sph, r17\n");

    //All that should be left is the return value, which should be at the top of the stack.

}


