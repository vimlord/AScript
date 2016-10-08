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

/**
 * Gets the content of opening and closing chars
 * str   - The string starting at the char after a
 *         char equal to start
 * start - An opening character
 * end   - A closing character
 */
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

/**
 * Gets everything up until a certain operator, as long as there
 * is closure in the string as definied by the up and down characters
 */
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

int idxOfMathOp(char* str) {
    int len = 0;

    while(str[len]) {
        if(str[len] == '+' || str[len] == '*')
            break;
        else if(str[len] == '(')
            len += indexOfClosingChar(&str[len], '(', ')');
        
        if(str[len]) len++;
    }

    return len;
}

void pemdas(FILE* execfile, char* calc, int dst) {
    
    //printf("%s\n", calc);

    //Get the index of \0
    int i = 0;
    while(calc[i] != '\0') i++;
    
    
    if(i == 0) {
        //Fills the address with 0.
        loadReg(execfile, 16, "$0");
        copyRegToMem(execfile, dst, 16);
        return;
        
    } else if(*calc == ' ') {
        int i = 1;
        while(calc[i] == ' ') i++;

        pemdas(execfile, &calc[i], dst);
        
        return;
    }

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
    
    static char opSymbols[] = "|&+-*";

    static MathOperation operations[5] = { bitOrOperation, bitAndOperation, addOperation, subOperation, mulOperation };
    
    //Parses through operators
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
   
    if(*calc == '(') {

        //There is something in parentheses here.
        char* parcont = parenthesesContent(&calc[1]);
        pemdas(execfile, parcont, dst);

        i = 0;
        while(parcont[i]) parcont[i++] = '\0';
        free(parcont);

        return;
    }
    
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
    //It will be stored in var
    loadReg(execfile, 16, calc);
    copyRegToMem(execfile, dst, 16);

}

void jumpIfTrue(FILE* execfile, char* cond, char* label, int stkptr) {
     
    writeComment(execfile, "Compute conditional");
    pemdas(execfile, cond, stkptr);
    
    writeComment(execfile, "Get values");
    //Copies the value into registers
    copyRegFromMem(execfile, 0x10, stkptr);
    loadReg(execfile, 0x11, "$0");
    
    writeComment(execfile, "Branch if nonzero");
    //Branches if equal to zero (0 is false)
    branchNE(execfile, 0x10, 0x11, label);


}

void jumpIfFalse(FILE* execfile, char* cond, char* label, int stkptr) {
    
    writeComment(execfile, "Compute conditional");
    pemdas(execfile, cond, stkptr);
    
    writeComment(execfile, "Get values");
    //Copies the value into registers
    copyRegFromMem(execfile, 0x10, stkptr);
    loadReg(execfile, 0x11, "$0");
    
    writeComment(execfile, "Branch if zero");
    //Branches if equal to zero (0 is false)
    branchEQ(execfile, 0x10, 0x11, label);
}
