#include "srccompile.h"

#include "asmcommands.h"
#include "mathconvert.h"

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
    
    //The address of the new memory
    int ptr = vars;

    //A buffer that holds the next line(s) of assembly. 
    int i = 0;
    while(i++ < nbytes)
        addToList(getVars(), varname);
    
    vars += nbytes;

    /*
    char varline[128];
    
    //Defines a variable w/ initial value of 0.
    sprintf(varline, "ldi r16, 0\nsts 0x%x, r16\n", vars + 0x0100);
    
    writeAsmBlock(stkfile, varline);
    */

    return ptr; 

}
void parseSegment(FILE* execfile, char* code) {
    char* nextLine = NULL;
    char* front = code;
    
    //Stores the layer depth of the program.
    static int depth = 0;

    depth++;

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

    depth--;

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

            processToken(execfile, TOKENS[i],
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
            //int memAddrs = 0x0100 + listSize(getVars());

            //By default, no equals sign means that the value will be set to 0.
            while(tokidx[idx] && tokidx[idx] != '=') idx++;
             
            pemdas(execfile, tokidx[idx+1] ? &tokidx[idx+1] : "0");
            
            //Copies value into slot
            stackPop(execfile, 0x10);
            copyRegToMem(execfile, 0x0100 + i, 0x10);

            return;
        }
        
        i++;
    }

    //If nothing has been chosen at this point, there's a syntax error.
    printf("Error during compilation: Unable to find token or variable.\n");
    exit(EINVAL);

}

void processToken(FILE* execfile, CMP_TOK tok, char* subline) {
    
    static int TOKEN_ID = 0;
    int tokenid = -1;

    if(*subline == ' ') {
        int i = 0;
        while(subline[i] == ' ')
            i++;
        
        processToken(execfile, tok, &subline[i]);
        return;
    } else {
        tokenid = TOKEN_ID++;
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
        addVariable(execfile, tok, varname, 1);

        //The memory address of the new variable.
        int valIdx = 0x0100 + listSize(getVars()) - 1;
        
        //If followed by an equal sign, include a definition for the variable.
        while(subline[i] == ' ') i++;
        
        if(subline[i] == '=') {
            
            //Since the variable is in the end slot, we can have
            //pemdas() push the values directly there.

            pemdas(execfile, subline[i+1] ? &subline[i+1] : "0");
            
            stackPop(execfile, 16);
    
        } else
           loadReg(execfile, 16, "$0");

        copyRegToMem(execfile, valIdx, 16);
                
    } else if(compTok(tok, "if") == 0) {
        
        //Get the if condition
        int len = 0;
        while(subline[len] != '(') len++;
        
        char* condition = closureContent(&subline[len+1], '(', ')');
        len += strlen(condition) + 1; //Len therefore hold the idx of the closing parenthesis.
        
        //Creates the text for the else label
        char elseLabel[64];
        sprintf(elseLabel, "else%i", tokenid);
       
        jumpIfFalse(execfile, condition, elseLabel);
        
        //Otherwise, execute
        //Gets the code block to run
        while(subline[len] != '{') len++;

        char* function = closureContent(&subline[len+1], '{', '}');

        parseSegment(execfile, function);
        
        //Generate the alternate function
        char* elseFunction = &subline[len+2] + strlen(function);
        while(*elseFunction == ' ') elseFunction = &elseFunction[1];

        char modlabel[64];
        
        //Jumps out of the if loop
        sprintf(modlabel, "endif%i", tokenid);
        jumpToLabel(execfile, modlabel);

        //Place the else label
        sprintf(modlabel, "%s:\n", elseLabel);       
        writeAsmBlock(execfile, modlabel);
        
        if(strstr(elseFunction, "else") == elseFunction) {
            elseFunction = &elseFunction[4];
            while(*elseFunction == ' ') elseFunction = &elseFunction[1];
            if(*elseFunction == '{') {
                //The else is a block
                char* elseBlock = closureContent(&elseFunction[1], '{', '}');
                parseSegment(execfile, elseBlock);
                free(elseBlock);
            } else {
                parseSegment(execfile, elseFunction); 
            }
        }
        
        sprintf(modlabel, "endif%i:\n", tokenid);
        writeAsmBlock(execfile, modlabel);

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
        jumpIfFalse(execfile, condition, endWhileLabel);
        
        //Otherwise, execute
        //Gets the code block to run
        while(subline[len] != '{') len++;

        char* function = closureContent(&subline[len+1], '{', '}');
        
        //Have the segment run
        parseSegment(execfile, function);

        //Adds the backwards jump to the start
        jumpToLabel(execfile, whileLabel);
        
        //Exit label
        sprintf(modLabel, "end%s:\n", whileLabel);
        writeAsmBlock(execfile, modLabel);
        
        free(condition);
        free(function);
    }

}




