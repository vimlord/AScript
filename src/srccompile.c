#include "srccompile.h"

#include "asmcommands.h"
#include "error.h"
#include "mathconvert.h"
#include "strmanip.h"
#include "tokens.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

char* TOKENS[] = {"if", "while", "byte", "ptr", "function", "return", ""};
TokenProcess TOKFUNCS[] = {
    processIfElse,
    processWhileLoop,
    processByte,
    processPtr,
    processFunction,
    handleReturn
};

//A singleton list that holds the program variables
List VARIABLES = NULL;
int REL_STK = 0; //Holds the relative position of the next item on the stack frame

int LOOP_DEPTH = -1;
int LOOP_DEPTH_GLOBAL = 0;
int LOOP_DEPTH_FUNCTION = 1;

List getVars() {
    return VARIABLES ? VARIABLES : (VARIABLES = makeList());
}

int getLoopDepth() {
    return LOOP_DEPTH;
}

/**
 * Gets the address of a variable in the stack frame
 */
int stackAddressOfVar(char* var) {
    List list = getVars();
    int i = 0, len = listSize(list);

    while(i < len) {
        VarFrame frame = (VarFrame) getFromList(list, i);

        if(!strcmp(var, frame->name))
            return frame->addr;
        
        i++;
    }

    return -131072;

}

/**
 * Returns the size of the variable type, or -1 if invalid.
 */
int sizeOfType(char* type) {
    if (!strcmp(type, "byte"))
         return 1;
    else if (!strcmp(type, "ptr"))
        return 2;
    else
        return -131072; //Outside the range of 16-bit address space
}

char* variableTypeOf(char* var) {
    List list = getVars();
    int i = 0, len = listSize(list);

    while(i < len) {
        VarFrame frame = (VarFrame) getFromList(list, i);

        if(!strcmp(var, frame->name))
            return frame->type;

        i++;
    }

    return NULL;
}

int variableSizeOf(char* var) {
    List list = getVars();
    int i = 0, len = listSize(list);

    while(i < len) {
        VarFrame frame = (VarFrame) getFromList(list, i);
        
        if(!strcmp(var, frame->name)) {
            return sizeOfType(frame->type);
        } else i++;
    }
    return -1;
}

void loadStackAddressOf(FILE* execfile, char* var) {
    char* name = contentToOperator(var, '[', '\0', '\0');
    char* tmp = &var[strlen(name)];
    while(*tmp == ' ') tmp = &tmp[1];
    char* arrIndex = NULL;
    if(*tmp == '[')
        arrIndex = closureContent(&tmp[1], '[', ']');

    int stkptr = stackAddressOfVar(name);
    writeAsmBlock(execfile, "mov yh, xh\nmov yl, xl\n");

    char buffer[64];
    sprintf(buffer, "ldi r16, %i\nsub yl, r16", stkptr % 256);
    writeAsmBlock(execfile, buffer);

    sprintf(buffer, "ldi r16, %i\nsbc yl, r16", stkptr / 256);
    writeAsmBlock(execfile, buffer);
    
    if(arrIndex) {
        //Compute the array index and store it in r16
        pemdas(execfile, arrIndex, 1);
        stackPop(execfile, 16);
        
        //Goes to the address
        writeAsmBlock(execfile, "add yl, r16\nldi r16, 0\nadc yh, r16\n");

        free(arrIndex);
    }

    free(name);
}

int compTok(CMP_TOK a, CMP_TOK b) {
    return strcmp(a, b);
}

void setCompilerStackTop(int idx) {
    REL_STK = idx;
}

int addVariable(FILE* execfile, CMP_TOK type, char* varname, int nbytes) {
    
    loadReg(execfile, 16, "0");

    //The address of the new memory
    int ptr = (REL_STK += nbytes) - 1;
    
    VarFrame frame = (VarFrame) malloc(sizeof(struct var_frame));
    frame->name = varname;
    frame->type = type;
    frame->addr = ptr;

    addToList(getVars(), frame);
    
    return ptr; 

}

void parseSegment(FILE* execfile, char* code) {
    
    char* nextLine = NULL;
    char* front = code;
    
    //Stores the layer depth of the program.
    LOOP_DEPTH++;
    
    //Holds the old size of the list
    int numvars = listSize(getVars());
    
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

    int endvars = listSize(getVars());
    
    //Remove variables if necessary in a function based on scope.
    if(LOOP_DEPTH >= LOOP_DEPTH_FUNCTION && endvars > numvars) {

        while(endvars > numvars) {
            VarFrame v = remFromList(getVars(), endvars-1);
            free(v->name);
            free(v);
            endvars--;
        }

        writeAsmBlock(execfile, "mov yh, xh\nmov yl, xl\n");
        
        //Get the stack pointer before the segment
        int stkaddrs = numvars ? ((VarFrame) getFromList(getVars(), numvars-1))->addr : 0;
        char buffer[128];
        sprintf(buffer, "ldi r16, %i\nsub yl, r16\n", stkaddrs);
        writeAsmBlock(execfile, buffer);

        //Reset the stack pointer
        writeAsmBlock(execfile, "out sph, yh\nout spl, yl\n");
    }
    LOOP_DEPTH--;

}

void parseLine(FILE* execfile, char* line) {

    if(!(*line))
        return;
    else {
        writeComment(execfile, line);
    }
    
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
    int len = listSize(getVars());
    while(i < len) {
        VarFrame varframe = (VarFrame) getFromList(getVars(), i);
        char* varname = varframe->name;
        
        //Searches for the variable in the list of variables
        if(!strcmp(varname, variable)) {
            if(!strcmp(varframe->type, "byte"))
                processByteAssign(execfile, line, varname, arrIdxStr);
            else if(!strcmp(varframe->type, "ptr"))
                processPtrAssign(execfile, line, varname, arrIdxStr);
            free(variable);
            return;
        }
        
        i++;
    }

    //If nothing has been chosen at this point, there's a syntax error.
    char* helpMssg = contentToOperator(line, '{', '\0', '\0');
    throwError("Unable to find token or variable:\n%s\n", helpMssg);

}


void processToken(FILE* execfile, int tokidx, char* subline) {
    
    static int TOKEN_ID = 0;
    int tokenid = -1;

    if(*subline == ' ') {
        int i = 1;
        while(subline[i] == ' ')
            i++;
        
        processToken(execfile, tokidx, &subline[i]);
        return;
    } else {
        tokenid = TOKEN_ID++;
    }

    TOKFUNCS[tokidx](execfile, subline, tokenid);

}




