#include "mathconvert.h"

#include "asmcommands.h"
#include "pemdas.h"
#include "srccompile.h"
#include "strmanip.h"

#include <string.h>
#include <stdlib.h>

void pemdas(FILE* execfile, char* calc) {

    if(!(*calc)) {
        //Fills the address with 0 if the string is empty.
        loadReg(execfile, 16, "0");
        stackPush(execfile, 16);
        return;
        
    } else if(*calc == ' ') {
        //If there are leading spaces, remove them and restart
        int i = 1;
        while(calc[i] == ' ') i++;

        pemdas(execfile, &calc[i]);
        
        return;
    }
    
    //printf("CALC: %s\n", calc);

    //Get the index of \0
    int i = 0;
    while(calc[i] != '\0') i++;
     
    //If there are any spaces at the end, remove them. 
    if(calc[i-1] == ' ') {
        i--;
        while(calc[i-1] == ' ') i--;

        char* newCalc = (char*) malloc(i * sizeof(char));
        int j = 0;
        while(j < i) {
            newCalc[j] = calc[j];
            j++;
        }
        newCalc[j] = '\0';
        
        pemdas(execfile, newCalc);

        return;

    }
    
    //These values are preserved, and are used by the program to properly execute PEMDAS.
    static char opSymbols[] = "|^&<>=!+-*";
    static MathOperation operations[10] = {
        bitOrOperation,
        bitXorOperation,
        bitAndOperation,
        boolLtOperation,
        boolGtOperation,
        boolEqOperation,
        boolNeOperation,
        addOperation,
        subOperation,
        mulOperation
    };

    
    //Parses through operators, and chooses the appropriate operation to run.
    i = 0;
    while(opSymbols[i]) {
        char* partA = contentToOperator(calc, opSymbols[i], '(', ')');

        if(strcmp(partA, calc)) {
            
            char* partB = &calc[strlen(partA) + 1];
            
            MathOperation mathOp = operations[i];

            mathOp(execfile, partA, partB);

            int j = 0;
            while(partA[j]) partA[j++] = '\0';
            free(partA);
        
            return;
        } else {
            int j = 0;
            while(partA[j]) partA[j++] = '\0';
            free(partA);

            i++;
        }
    }
    
    //Drops parentheses if the entire statement is a parenthetical.
    if(*calc == '(') {
        char* parcont = parenthesesContent(&calc[1]);
        pemdas(execfile, parcont);

        i = 0;
        while(parcont[i]) parcont[i++] = '\0';
        free(parcont);

        return;
    }
    
    //Try to find a matching variable.
    //List vars = getVars();

    //Finds length of the variable name 
    i = 0;
    while(calc[i] && calc[i] != ' ' && calc[i] != '[') i++;
    char* variableName = (char*) malloc((i+1) * sizeof(char));
    int j = -1;
    while(++j < i)
        variableName[j] = calc[j];
    variableName[i] = '\0';

    //Gets the array index, if there is one
    char* arrIdxStr = NULL;
    if(calc[i] == '[')
        arrIdxStr = closureContent(&calc[i+1], '[', ']');

    //i will hold the index in the stack of the variable
    i = stackAddressOfVar(variableName);

    if(i >= 0) {
        //The variable exists on the stack frame

        //Copies the stack pointer to y 
        writeAsmBlock(execfile, "mov yh, xh\nmov yl, xl\n");
        
        char addrBuffer[64];

        //Gets index on stack
        sprintf(addrBuffer, "ldi r16, %i\nsub yl, r16\n", i % 256);
        writeAsmBlock(execfile, addrBuffer);

        if(arrIdxStr) {
            writeComment(execfile, "Getting array index");
            //The access is done as an array
            
            //First, we need the index
            pemdas(execfile, arrIdxStr);
            //Next, we pop the value off of the stack
            stackPop(execfile, 16);

            //Then, we need to add it to the zero memory address
            addReg(execfile, 0x1d, 0x10); //The end result is that y now contains the address of the index
        }
        
        //Copies the variable's value from the address in y into r16.
        writeAsmBlock(execfile, "ld r16, y\n");

        //Then, push the value onto the stack.
        stackPush(execfile, 16);
        return;
    }
    
    //If the program reached this point, then there has to be a scalar here.
    //It will be stored in dst
    loadReg(execfile, 16, calc);
    stackPush(execfile, 16);
}

void jumpIfTrue(FILE* execfile, char* cond, char* label) {
     
    writeComment(execfile, "Compute conditional");
    pemdas(execfile, cond);
    
    writeComment(execfile, "Get values");
    //Copies the value into registers
    stackPop(execfile, 16);
    loadReg(execfile, 17, "$0");

    writeComment(execfile, "Branch if nonzero");
    //Branches if equal to zero (0 is false)
    branchNE(execfile, 0x10, 0x11, label);


}

void jumpIfFalse(FILE* execfile, char* cond, char* label) {
    
    writeComment(execfile, "Compute conditional");
    pemdas(execfile, cond);
    
    writeComment(execfile, "Get values");
    //Copies the value into registers
    stackPop(execfile, 16);
    loadReg(execfile, 17, "0");

    writeComment(execfile, "Branch if zero");
    //Branches if equal to zero (0 is false)
    branchEQ(execfile, 0x10, 0x11, label);
}
