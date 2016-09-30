#include "srccompile.h"
#include "asmcommands.h"

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
    char varline[128];

    printf("Adding var at 0x%x called '%s'\n", vars + 0x0100, varname);
    
    //Creates the variable list if it doesn't already exist.

    addToList(getVars(), varname);

    //Defines a variable w/ initial value of 0.
    sprintf(varline, "ldi r16, 0\nsts 0x%x, r16\n", vars + 0x0100);
    
    writeAsmBlock(stkfile, varline);

    return vars++; 

}

void parseLine(FILE* stkfile, FILE* execfile, char* line) {

    printf("\nChecking line: %s\n", line);
    //Will test for the index of the first token
    char* tokidx;
    
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
        }
        
        i++;
    }

}

void processToken(FILE* stkfile, FILE* execfile, CMP_TOK tok, char* subline) {
    


    if(!compTok(tok, "byte")) {
        printf("Adding a new variable of type '%s'\n", tok);
        
        //Stores the var name in a variable.
        int len = 0;
        while(subline[len] != ' ') len++;
        char* varname = (char*) malloc((len + 1) * sizeof(char));
        
        varname[len] = '\0';
        int i = 0;
        while(i < len) {
            varname[i] = subline[i];
            i++;
        }

        //Adds the variable.
        addStackFrameVar(stkfile, tok, 0, varname);

        //If a variable is followed by a definition
        //the code for doing so should be generated.
        parseLine(stkfile, execfile, subline);

    }

}




