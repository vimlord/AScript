#include "mathconvert.h"

#include "asmcommands.h"
#include "pemdas.h"
#include "srccompile.h"

#include <string.h>
#include <stdlib.h>

int indexOfClosingChar(char* str, char start, char end) {
    int len = 0, i = 0;
    while(str[len]) {
        if(str[len] == start)
            i++;
        else if(str[len] == end) {
            if(i)
                i--;
            else
                break;
        }
        len++;
    }

    return str[len] ? -1 : len;
}

char* closureContent(char* str, char start, char end) {
    int len = 0, i = 0;
    while(str[len]) {
        if(str[len] == start)
            i++;
        else if(str[len] == end) {
            if(i)
                i--;
            else
                break;
        }
        len++;
    }
    
    //The inveriant is that len is at the end of the content
    char* result = (char*) malloc((len+1) * sizeof(char));
    i = 0;
    while(i < len) {
        result[i] = str[i];
        i++;
    }
    result[i] = '\0';

    return result;

}

char* parenthesesContent(char* start) {
    return closureContent(start, '(', ')');
}

char* bracketContent(char* start) {
    return closureContent(start, '{', '}');
}

char* contentToOperator(char* start, char op, char up, char down) {
    int len = 0;

    while(start[len] != '\0') {
        if(start[len] == op)
            break;
        else if(start[len] == '(')
            len += indexOfClosingChar(&start[len], up, down);
        
        len++;

    }
    
    char* result = (char*) malloc((len + 1) * sizeof(char));
    int i = -1;
    while(++i < len)
        result[i] = start[i];
    result[i] = '\0';

    return result;
}

void pemdas(FILE* execfile, char* calc, int dst) {
    
    //printf("%s\n", calc);

    if(!(*calc)) {
        //Fills the address with 0 if the string is empty.
        loadReg(execfile, 16, "$0");
        copyRegToMem(execfile, dst, 16);
        return;
        
    } else if(*calc == ' ') {
        //If there are leading spaces, remove them and restart
        int i = 1;
        while(calc[i] == ' ') i++;

        pemdas(execfile, &calc[i], dst);
        
        return;
    }

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
        
        pemdas(execfile, newCalc, dst);

        return;

    }
    
    //These values are preserved, and are used by the program to properly execute PEMDAS.
    static char opSymbols[] = "|&+-*";
    static MathOperation operations[5] = { bitOrOperation, bitAndOperation, addOperation, subOperation, mulOperation };
    
    //Parses through operators, and chooses the appropriate operation to run.
    i = 0;
    while(opSymbols[i]) {
        char* partA = contentToOperator(calc, opSymbols[i], '(', ')');

        if(strcmp(partA, calc)) {
            
            char* partB = &calc[strlen(partA) + 1];
            
            MathOperation mathOp = operations[i];

            mathOp(execfile, partA, partB, dst);

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
        pemdas(execfile, parcont, dst);

        i = 0;
        while(parcont[i]) parcont[i++] = '\0';
        free(parcont);

        return;
    }
    
    //Try to find a matching variable.
    List vars = getVars();
    int numVars = listSize(vars);
    
    i = 0;
    while(i < numVars) {
        char* testVar = getFromList(vars, i);
        
        if(strcmp(testVar, calc) == 0) {

            //testVar is the variable we seek.
            
            /*
             * Copy the register to the destination.
             * The variable will be at 0x0100 + i
             */
            copyRegFromMem(execfile, 16, 0x0100 + i);
            copyRegToMem(execfile, dst, 16);
            
            return;
        } else i++;

    }
    
    //If the program reached this point, then there has to be a scalar here.
    //It will be stored in dst
    loadReg(execfile, 16, calc);
    copyRegToMem(execfile, dst, 16);

}

void jumpIfTrue(FILE* execfile, char* cond, char* label, int ptr) {
     
    writeComment(execfile, "Compute conditional");
    pemdas(execfile, cond, ptr);
    
    writeComment(execfile, "Get values");
    //Copies the value into registers
    copyRegFromMem(execfile, 0x10, ptr);
    loadReg(execfile, 0x11, "$0");
    
    writeComment(execfile, "Branch if nonzero");
    //Branches if equal to zero (0 is false)
    branchNE(execfile, 0x10, 0x11, label);


}

void jumpIfFalse(FILE* execfile, char* cond, char* label, int ptr) {
    
    writeComment(execfile, "Compute conditional");
    pemdas(execfile, cond, ptr);
    
    writeComment(execfile, "Get values");
    //Copies the value into registers
    copyRegFromMem(execfile, 0x10, ptr);
    loadReg(execfile, 0x11, "$0");
    
    writeComment(execfile, "Branch if zero");
    //Branches if equal to zero (0 is false)
    branchEQ(execfile, 0x10, 0x11, label);
}
