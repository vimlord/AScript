#include "mathconvert.h"

#include "asmcommands.h"
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
    while(i < len)
        result[i] = str[i];
    result[i] = '\0';

    return result;

}

char* parenthesesContent(char* start) {
    return closureContent(start, '(', ')');
}

char* bracketContent(char* start) {
    return closureContent(start, '{', '}');
}

char* contentToOperator(char* start, char op) {
    int len = 0;

    while(start[len]) {
        if(start[len] == op)
            break;
        else if(start[len] == '(')
            len += indexOfClosingChar(&start[len], '(', ')');
        
        if(start[len]) len++;
    }

    char* result = (char*) malloc((len + 1) * sizeof(char));
    int i = 0;
    while(i < len)
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
    
    if(*calc == '\0') {
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

    //Addition
    char* partA = contentToOperator(calc, '+');
    if(strcmp(partA, calc)) {
        //There is an addition operation that can be done
        
        char* partB = &calc[strlen(partA) + 1];

        //Compute the two subcomponents
        pemdas(execfile, partA, dst + 1);
        pemdas(execfile, partB, dst + 2);

        //Grab the results
        copyRegFromMem(execfile, 16, dst + 1); //A
        copyRegFromMem(execfile, 17, dst + 2); //B

        //Add, then store.
        addReg(execfile, 16, 17);
        copyRegToMem(execfile, dst, 16);
        
        int i = 0;
        while(partA[i]) partA[i++] = '\0';
        free(partA);
        
        i = 0;
        while(partB[i]) partB[i++] = '\0';
        free(partB);
        
        return;
    }

    //Subtraction
    partA = contentToOperator(calc, '-');
    if(strcmp(partA, calc)) {
        //There is a subtraction operation that can be done
        
        char* partB = &calc[strlen(partA) + 1];

        //Compute the two subcomponents
        pemdas(execfile, partA, dst + 1);
        pemdas(execfile, partB, dst + 2);

        //Grab the results
        copyRegFromMem(execfile, 16, dst + 1); //A
        copyRegFromMem(execfile, 17, dst + 2); //B

        //Add, then store.
        subReg(execfile, 16, 17);
        copyRegToMem(execfile, dst, 16);
         
        int i = 0;
        while(partA[i]) partA[i++] = '\0';
        free(partA);
        
        i = 0;
        while(partB[i]) partB[i++] = '\0';
        free(partB);
        
        return;
    }
    
    //Multiplication
    partA = contentToOperator(calc, '*');
    if(strcmp(partA, calc)) {
        //There is a multiplication operation that can be done
        /* MULTIPLICATION IS INCOMPLETE */

        char* partB = &calc[strlen(partA) + 1];

        //Compute the two subcomponents
        pemdas(execfile, partA, dst + 1);
        pemdas(execfile, partB, dst + 2);

        //Grab the results
        copyRegFromMem(execfile, 16, dst + 1); //A
        copyRegFromMem(execfile, 17, dst + 2); //B

        //Mult, then store.
        mulRegs(execfile, 16, 17);
        copyRegToMem(execfile, dst, 0);
         
        int i = 0;
        while(partA[i]) partA[i++] = '\0';
        free(partA);
        
        i = 0;
        while(partB[i]) partB[i++] = '\0';
        free(partB);
        
        return;
    }

    if(*calc == '(') {
        //There is something in parentheses here.
        char* parcont = parenthesesContent(&calc[1]);
        pemdas(execfile, parcont, dst);

        int i = 0;
        while(parcont[i]) parcont[i++] = '\0';

        return;
    }



}


