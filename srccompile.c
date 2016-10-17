#include "srccompile.h"

#include "asmcommands.h"
#include "mathconvert.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

char* TOKENS[] = {"if", "while", "byte", ""};

//A singleton list that holds the program variables
List VARIABLES = NULL;
List ADDRESSES = NULL;

List getVars() {
    return VARIABLES ? VARIABLES : (VARIABLES = makeList());
}

List getStkAddrs() {
    return ADDRESSES ? ADDRESSES : (ADDRESSES = makeList());
}

/**
 * Gets the address of a variable in the stack frame
 */
int stackAddressOfVar(char* var) {
    List list = getVars();
    int idx = listIndexOf(list, var);
    if(idx < 0)
        return -1;
    else {
        int* ptr = (int*) getFromList(list, idx);
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

    void* voidPtr = (void*) ptr;
    printf("A: %p\nB: %p\n", (void*) ptr, voidPtr);
    addToList(getVars(), varname);
    addToList(getStkAddrs(), voidPtr); 
    
    return *ptr; 

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
    
    //printf("LINE: '%s'\n", line);

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

    //Finds the length of the variable name
    i = 0;
    while(line[i] && line[i] != ' ' && line[i] != '[') i++;

    //Gets the array index, if there is one
    char* arrIdxStr = NULL;
    if(line[i] == '[')
        arrIdxStr = closureContent(&line[i+1], '[', ']');
    
    i = 0;
    int len = listSize(VARIABLES);
    while(i < len) {
        char* varname = getFromList(VARIABLES, i);

        if((tokidx = strstr(line, varname)) == line) {
            //The variable is in the string

            int idx = (int) strlen(varname);

            //By default, no equals sign means that the value will be set to 0.
            while(tokidx[idx] && tokidx[idx] != '=') idx++;
             
            pemdas(execfile, tokidx[idx+1] ? &tokidx[idx+1] : "0");
            writeComment(execfile, "Getting array index");
            
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
            sprintf(addrBuffer, "ldi r16, $%x\nsub xl, r16\n", stkIdx % 256);
            writeAsmBlock(execfile, addrBuffer);

            if(arrIdxStr) {
                //Then, we calculate the index
                pemdas(execfile, arrIdxStr);
                //Next, we pop the value off of the stack
                stackPop(execfile, 16);

                //Then, we need to add it to the zero memory address
                addReg(execfile, 0x1d, 0x10);
            } 
            
            writeComment(execfile, "Editing array");
            //Finally, we get the variable and put it in the slot
            stackPop(execfile, 0x10);
            sprintf(addrBuffer, "st y, r16\n");
            writeAsmBlock(execfile, addrBuffer);


        
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
        
        int nbytes = 1; //The size of the list, in bytes (1 * size)
        int i = 0; //The index of the variable name's first char

        if(*subline == '[') {
            //Get the variable size from the string
            char* content = closureContent(&subline[1], '[', ']');
            nbytes = atoi(content);

            //Free the string
            i = 0;
            while(content[i]) {
                content[i] = '\0';
                i++;
            }

            //i should hold the index of the first char in the variable
            i += 2;
            while(subline[i] == ' ') i++;

            free(content);
        } else {
            nbytes = 1;
        }
        
        int len = 0;
        while(subline[len + i] != ' ' && subline[len + i]) len++;
           
        //Allocates space for the variable name
        char* varname = (char*) malloc((len+1) * sizeof(char));
        
        //Stores the var name in varname.
        varname[len] = '\0';
        int j = 0;
        while(j < len) {
            varname[j] = subline[j+i];
            j++;
        }
        i += len; 

        //Adds the variable.
        /*int valIdx = */addVariable(execfile, tok, varname, nbytes);

        //If followed by an equal sign, include a definition for the variable.
        while(subline[i] == ' ') i++;
        
        if(subline[i] == '=') {
            
            //Since the variable is in the end slot, we can have
            //pemdas() push the values directly there.
            
            pemdas(execfile, subline[i+1] ? &subline[i+1] : "0");
            
            stackPop(execfile, 16);
    
        } else
           loadReg(execfile, 16, "$0");
        
        //This ensures that if there is more than one item, they will all have the same value
        i = 0;
        while(i++ < nbytes)
            stackPush(execfile, 16);//copyRegToMem(execfile, valIdx + i++, 16);

                
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
        
        //writeAsmBlock(execfile, "\n");

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




