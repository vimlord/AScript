#include "srccompile.h"

#include "asmcommands.h"
#include "mathconvert.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

char* TOKENS[] = {"byte", NULL};

//A singleton list that holds the program variables
List VARIABLES = NULL;

List getVars() {
    return VARIABLES ? VARIABLES : (VARIABLES = makeList());
}

int compTok(CMP_TOK a, CMP_TOK b) {
    return strcmp(a, b);
}

int addStackFrameVar(FILE* stkfile, CMP_TOK type, int val, char* varname) {

    static int vars = 0;
    
    //A buffer that holds the next line(s) of assembly. 

    addToList(getVars(), varname);
    
    /*
    char varline[128];
    
    //Defines a variable w/ initial value of 0.
    sprintf(varline, "ldi r16, 0\nsts 0x%x, r16\n", vars + 0x0100);
    
    writeAsmBlock(stkfile, varline);
    */

    return vars++; 

}

void parseLine(FILE* stkfile, FILE* execfile, char* line) {
    
    //printf("LINE: %s\n", line);

    //Will test for the index of the first token
    char* tokidx = NULL;
    
    //Processes by token, if possible.
    int i = -1;
    while(TOKENS[++i]) {
        //If a token is found, process it.
        if((tokidx = strstr(line, TOKENS[i]))) {
            processToken(stkfile, execfile, TOKENS[i],
                         &tokidx[1 + strlen(TOKENS[i])]);
            //Nothing else needs to be done; return.
            return;
        } else {
            //printf("'%s' does not match.\n", TOKENS[i]);
        }
    }

    //Try for variable assignments
    i = 0;
    int len = listSize(VARIABLES);
    while(i < len) {
        char* varname = getFromList(VARIABLES, i);
        
        /* INCOMPLETE */

        if((tokidx = strstr(line, varname))) {
            //The variable is in the string
            
            int idx = (int) strlen(varname);
            int memAddrs = 0x0100 + listSize(getVars());

            //By default, no equals sign means that the value will be set to 0.
            while(tokidx[idx] && tokidx[idx] != '=') idx++;
             
            pemdas(execfile, tokidx[idx+1] ? &tokidx[idx+1] : "0", memAddrs);
            
            //Copies value into slot
            copyRegFromMem(execfile, 0x10, memAddrs);
            copyRegToMem(execfile, 0x0100 + i, 0x10);
        }
        
        i++;
    }

}

void processToken(FILE* stkfile, FILE* execfile, CMP_TOK tok, char* subline) {
    
    //printf("TOKEN: '%s'\nLINE: '%s'\n", tok, subline);

    if(compTok(tok, "byte") == 0) {
        
        //Stores the var name in a variable.
        int len = 0;
        while(subline[len] != ' ' && subline[len]) len++;
        char* varname = (char*) malloc((len + 1) * sizeof(char));
        
        varname[len] = '\0';
        int i = 0;
        while(i < len) {
            varname[i] = subline[i];
            i++;
        }
        
        //printf("Going to add.\n");

        //Adds the variable.
        addStackFrameVar(stkfile, tok, 0, varname);

        //The memory address of the new variable.
        int valIdx = 0x0100 + listSize(getVars()) - 1;
        
        //If followed by an equal sign, include a definition for the variable.
        while(subline[i] == ' ') i++;
        
        if(subline[i] == '=') {
            
            //Since the variable is in the end slot, we can have
            //pemdas() push the values directly there.

            pemdas(stkfile, subline[i+1] ? &subline[i+1] : "0", valIdx);
            
        } else {
            pemdas(stkfile, "0", valIdx); 
        }
        
    }

}




