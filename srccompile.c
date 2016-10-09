#include "srccompile.h"

#include "asmcommands.h"
#include "mathconvert.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

char* TOKENS[] = {"if", "while", "byte", ""};

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

void parseSegment(FILE* stkfile, FILE* execfile, char* line) {
    char* nextLine = NULL;
    char* front = line;
    
    static int depth = 0;

    depth++;

    while(*front) {
        
        //Parse the next line.
        nextLine = contentToOperator(front, ';', '{', '}');
         
        front = &front[strlen(nextLine)]; 
        while(*front == ';' || *front == ' ')
            front = &front[1];

        parseLine(stkfile, execfile, nextLine);

        writeAsmBlock(execfile, "\n");

    }

    depth--;

}

void parseLine(FILE* stkfile, FILE* execfile, char* line) {
    
    if(!(*line))
        return;
    else {
        writeComment(execfile, line);
        printf("LINE: '%s'\n", line);
    }
    
    //Will test for the index of the first token
    char* tokidx = NULL;
    
    //Processes by token, if possible.
    int i = 0;
    while(strcmp(TOKENS[i], "")) {

        //If a token is found, process it.
        tokidx = strstr(line, TOKENS[i]);
        
        if(tokidx == line) {

            processToken(stkfile, execfile, TOKENS[i],
                         &tokidx[strlen(TOKENS[i])]);
            //Nothing else needs to be done; return.

            return;
        }
        i++;
    }
    
    //Try for variable assignments
    i = 0;
    int len = listSize(VARIABLES);
    while(i < len) {
        char* varname = getFromList(VARIABLES, i);
        
        if((tokidx = strstr(line, varname)) == line) {
            //The variable is in the string

            int idx = (int) strlen(varname);
            int memAddrs = 0x0100 + listSize(getVars());

            //By default, no equals sign means that the value will be set to 0.
            while(tokidx[idx] && tokidx[idx] != '=') idx++;
             
            pemdas(execfile, tokidx[idx+1] ? &tokidx[idx+1] : "0", memAddrs);
            
            //Copies value into slot
            copyRegFromMem(execfile, 0x10, memAddrs);
            copyRegToMem(execfile, 0x0100 + i, 0x10);

            return;
        }
        
        i++;
    }

    //If nothing has been chosen at this point, there's a syntax error.
    printf("Error during compilation: Input file not found.\n");
    exit(EINVAL);

}

void processToken(FILE* stkfile, FILE* execfile, CMP_TOK tok, char* subline) {
    
    static int tokenid = -1;
    
    if(*subline == ' ') {
        int i = 0;
        while(subline[i] == ' ')
            i++;
        
        processToken(stkfile, execfile, tok, &subline[i]);
        return;
    } else {
        tokenid++;
    }

    if(compTok(tok, "byte") == 0) {
 
        int len = 0;
        while(subline[len] != ' ' && subline[len]) len++;
           
        //Stores the var name in a variable.
        char* varname = (char*) malloc((len + 1) * sizeof(char));
        
        varname[len] = '\0';
        int i = 0;
        while(i < len) {
            varname[i] = subline[i];
            i++;
        }
        
        //printf("Going to add.\n");

        //Adds the variable.
        addStackFrameVar(execfile, tok, 0, varname);

        //The memory address of the new variable.
        int valIdx = 0x0100 + listSize(getVars()) - 1;
        
        //If followed by an equal sign, include a definition for the variable.
        while(subline[i] == ' ') i++;
        
        if(subline[i] == '=') {
            
            //Since the variable is in the end slot, we can have
            //pemdas() push the values directly there.

            pemdas(execfile, subline[i+1] ? &subline[i+1] : "0", valIdx);
            
        } else {
            pemdas(execfile, "0", valIdx); 
        }
        
    } else if(compTok(tok, "if") == 0) {
        
        printf("%s\n", subline);

        //Get the if condition
        int len = 0;
        while(subline[len] != '(') len++;
        
        char* condition = closureContent(&subline[len+1], '(', ')');
        len += strlen(condition) + 1; //Len therefore hold the idx of the closing parenthesis.
        
        //Creates the text for the else label
        char elseLabel[64];
        sprintf(elseLabel, "else%i", tokenid);
       
        jumpIfFalse(execfile, condition, elseLabel, 0x0100 + listSize(getVars()));
        
        //Gets the code block to run
        while(subline[len] != '{') len++;
        char* function = closureContent(&subline[len+1], '{', '}');
        
        parseSegment(stkfile, execfile, function);

        char modLabel[64];
        sprintf(modLabel, "%s:\n", elseLabel);
        writeAsmBlock(execfile, modLabel);

        free(condition);
        free(function);
    } else if(compTok(tok, "while") == 0) {
        
        //Get the while condition
        int len = 0;
        while(subline[len] != '(') len++;
        
        char* condition = closureContent(&subline[len+1], '(', ')');
        len += strlen(condition) + 1; //Len therefore hold the idx of the closing parenthesis.
        
        //Create the custom label for jumping
        char whileLabel[64];
        char modLabel[64];
        sprintf(whileLabel, "while%i", tokenid);

        //Makes the starting label for the while loop. The program jumps
        //here after every execution of the contents of the while loop
        sprintf(modLabel, "%s:\n", whileLabel);
        writeAsmBlock(execfile, modLabel);
        
        //Gets the exit label
        char endWhileLabel[64];
        sprintf(endWhileLabel, "end%s", whileLabel);

        //If false, exit the loop
        jumpIfFalse(execfile, condition, endWhileLabel, 0x0100 + listSize(getVars()));

        //Otherwise, execute
        //Gets the code block to run
        while(subline[len] != '{') len++;
        char* function = closureContent(&subline[len+1], '{', '}');
        
        //Have the segment run
        parseSegment(stkfile, execfile, function);

        //Adds the backwards jump to the start
        jumpToLabel(execfile, whileLabel);
        
        //Exit label
        sprintf(modLabel, "end%s:\n", whileLabel);
        writeAsmBlock(execfile, modLabel);
        
        free(condition);
        free(function);
    }

}




