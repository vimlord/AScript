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

void pemdas(FILE* execfile, char* calc) {
    
    printf("CALC: %s\n", calc);

    if(!(*calc)) {
        //Fills the address with 0 if the string is empty.
        loadReg(execfile, 16, "$0");
        stackPush(execfile, 16);
        return;
        
    } else if(*calc == ' ') {
        //If there are leading spaces, remove them and restart
        int i = 1;
        while(calc[i] == ' ') i++;

        pemdas(execfile, &calc[i]);
        
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
    List vars = getVars();
    int numVars = listSize(vars);
    

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

    //Iterate through all variables
    i = 0;
    while(i < numVars) {
        char* testVar = getFromList(vars, i);
        
        if(strcmp(testVar, variableName) == 0) {
            
            printf("Full name '%s'\n", calc);
            printf("Will get var val '%s'\n", variableName);
            if(arrIdxStr) {
                printf("Index of '%s'\n", arrIdxStr);
            }

            //testVar is the variable we seek.
            
            /*
             * Copy the register to the destination.
             * The variable will be at 0x0100 + i
             */
            if(arrIdxStr) {
                
                printf("Reading from array.\n");

                writeComment(execfile, "Getting array index");
                //The access is done as an array
                //First, calculate the address
                char addrBuffer[32];
                //We will need the zero index
                sprintf(addrBuffer, "ldi zh, $%x\nldi zl, $%x\n", 1 + (i / 256), i % 256);
                writeAsmBlock(execfile, addrBuffer);
                
                //Then, we calculate the index
                pemdas(execfile, arrIdxStr);
                //Next, we pop the value off of the stack
                stackPop(execfile, 16);

                //Then, we need to add it to the zero memory address
                addReg(execfile, 0x1f, 0x10);
                
                writeComment(execfile, "Reading from array");
                //Finally, we get the variable and put it in the slot
                writeAsmBlock(execfile, "ld r16, z\n"); //Pull from memory
            } else
                copyRegFromMem(execfile, 16, 0x0100 + i);
            
            stackPush(execfile, 16);

            return;
        } else i++;

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
    loadReg(execfile, 17, "$0");

    writeComment(execfile, "Branch if zero");
    //Branches if equal to zero (0 is false)
    branchEQ(execfile, 0x10, 0x11, label);
}
