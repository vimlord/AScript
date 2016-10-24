#include "srccompile.h"

#include "asmcommands.h"
#include "error.h"
#include "mathconvert.h"
#include "strmanip.h"
#include "tokens.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

char* TOKENS[] = {"if", "while", "byte", ""};
TokenProcess TOKFUNCS[] = {
    processIfElse,
    processWhileLoop,
    processByte
};

//A singleton list that holds the program variables
List VARIABLES = NULL;
List ADDRESSES = NULL;

int LOOP_DEPTH = 0;

List getVars() {
    return VARIABLES ? VARIABLES : (VARIABLES = makeList());
}

List getStkAddrs() {
    return ADDRESSES ? ADDRESSES : (ADDRESSES = makeList());
}

int getLoopDepth() {
    return LOOP_DEPTH;
}

/**
 * Gets the address of a variable in the stack frame
 */
int stackAddressOfVar(char* var) {
    List list = getVars();
    int idx = listIndexOfStr(list, var);
    if(idx < 0)
        return -1;
    else {
        int* ptr = (int*) getFromList(getStkAddrs(), idx);
        return *ptr;
    }
}

int compTok(CMP_TOK a, CMP_TOK b) {
    return strcmp(a, b);
}


/**
 * Adds a variable to the heap.
 *
 * execfile - The file containing the instructions.
 * type     - The type of variable being added.
 * val      - The initial value.
 * varname  - The name of the variable.
 *
 * Returns the address of the variable in memory.
 */
int addVariable(FILE* execfile, CMP_TOK type, char* varname, int nbytes) {

    static int vars = 0;
    
    loadReg(execfile, 16, "0");

    //The address of the new memory
    int* ptr = (int*) malloc(sizeof(int));
    *ptr = (vars += nbytes) - 1;

    addToList(getVars(), varname);
    addToList(getStkAddrs(), ptr);

    return *ptr; 

}

void parseSegment(FILE* execfile, char* code) {
    char* nextLine = NULL;
    char* front = code;
    
    //Stores the layer depth of the program.
    LOOP_DEPTH++;

    while(*front) {
        
        //Parse the next line (should have closure).
        nextLine = contentToOperator(front, ';', '{', '}');
        
        //Ensures that there are no leading spaces
        front = &front[strlen(nextLine)]; 
        while(*front == ';' || *front == ' ')
            front = &front[1];

        parseLine(execfile, nextLine);

        //Adds a newline for readability.
        writeAsmBlock(execfile, "\n");

    }

    LOOP_DEPTH--;

}

void parseLine(FILE* execfile, char* line) {

    if(!(*line))
        return;
    else {
        writeComment(execfile, line);
    }
    
    //printf("LINE: '%s'\n", line);

    //Will test for the index of the first token
    char* tokidx = NULL;
    
    //Processes by token, if possible.
    int i = 0;
    while(strcmp(TOKENS[i], "")) {

        //If a token is found, process it.
        tokidx = strstr(line, TOKENS[i]);
        
        if(tokidx == line) {

            processToken(execfile, i,
                         &tokidx[strlen(TOKENS[i])]);
            //Nothing else needs to be done; return.

            return;
        }
        i++;
    }
    
    //Try for variable assignments

    //Finds the length of the variable name
    i = 0;
    while(line[i] && line[i] != ' ' && line[i] != '[' && !strchr("= ", line[i])) i++;
    
    //Holds the variable name
    char* variable = malloc((i+1) * sizeof(char));
    int j = 0;
    while(j < i) {
        variable[j] = line[j];
        j++;
    }
    variable[j] = '\0';

    //Gets the array index, if there is one
    char* arrIdxStr = NULL;
    if(line[i] == '[')
        arrIdxStr = closureContent(&line[i+1], '[', ']');
    
    i = 0;
    int len = listSize(VARIABLES);
    while(i < len) {
        char* varname = getFromList(VARIABLES, i);
        
        //Searches for the variable in the list of variables
        if(!strcmp(varname, variable)) {
            
            tokidx = strstr(line, varname);

            //The variable is in the string

            int idx = (int) strlen(varname);

            //By default, no equals sign means that the value will be set to 0.
            while(tokidx[idx] && tokidx[idx] != '=') idx++;
            writeComment(execfile, "Computing value"); 
            pemdas(execfile, tokidx[idx+1] ? &tokidx[idx+1] : "0");
            writeComment(execfile, "Determining pointer address");
            
            //The access is done as an array
            //First, calculate the address
            char addrBuffer[64];
            //We will need the zero index of the stack
            //(x always holds index 0 of the stack)
            writeAsmBlock(execfile, "ld yh, xh\nld yl, xl\n");
            //Gets index on stack
            int stkIdx = stackAddressOfVar(varname);
            if(stkIdx < 0) {
                //The variable does not exist on the stack.
                printf("Error during compilation: A variable was not found.\n");
                exit(EINVAL);
            }
            sprintf(addrBuffer, "ldi r16, %i\nsub yl, r16\n", stkIdx % 256);
            writeAsmBlock(execfile, addrBuffer);
            
            if(arrIdxStr) {
                writeComment(execfile, "Computing array index");
                //Then, we calculate the index
                pemdas(execfile, arrIdxStr);
                //Next, we pop the value off of the stack
                stackPop(execfile, 16);

                //Then, we need to add it to the zero memory address
                addReg(execfile, 0x1d, 0x10);
            } 
            
            writeComment(execfile, "Storing end result");

            //Finally, we get the variable and put it in the slot
            stackPop(execfile, 0x10);
            sprintf(addrBuffer, "st y, r16\n");
            writeAsmBlock(execfile, addrBuffer);
            
            free(variable); 

            return;
        }
        
        i++;
    }

    //If nothing has been chosen at this point, there's a syntax error.
    throwError("Unable to find token or variable.");

}

void processToken(FILE* execfile, int tokidx, char* subline) {
    
    static int TOKEN_ID = 0;
    int tokenid = -1;

    if(*subline == ' ') {
        int i = 0;
        while(subline[i] == ' ')
            i++;
        
        processToken(execfile, tokidx, &subline[i]);
        return;
    } else {
        tokenid = TOKEN_ID++;
    }
    
    TOKFUNCS[tokidx](execfile, subline, tokenid);

}




