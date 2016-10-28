#include "tokens.h"

#include "asmcommands.h"
#include "error.h"
#include "mathconvert.h"
#include "srccompile.h"
#include "strmanip.h"

#include <string.h>
#include <stdlib.h>

void processByte(FILE* execfile, char* subline, int tokenid) {
    
    int nbytes = 1; //The size of the list, in bytes (1 * size)
    int i = 0; //The index of the variable name's first char
    
    //Creating variables in a loop might not be appreciated
    if(getLoopDepth() > 1) {
        char buffer[50 + strlen(subline)];
        sprintf(buffer, "Attempting to create byte inside loop:\n%s\n", subline);
        throwWarning(buffer);
    }

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
    
    //There should be a variable name
    if(!subline[i] || subline[i] == '=') {
        throwError("Attempting to create byte without name.\n");
    }

    int len = 0;
    while(subline[len + i] != ' ' && subline[len + i] && subline[len + i] != '=') len++;
     
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
    /*int valIdx = */addVariable(execfile, "byte", varname, nbytes);

    //If followed by an equal sign, include a definition for the variable.
    while(subline[i] == ' ') i++;
    
    if(subline[i] == '=') {
        
        //Since the variable is in the end slot, we can have
        //pemdas() push the values directly there.
        
        pemdas(execfile, subline[i+1] ? &subline[i+1] : "0");
        
        stackPop(execfile, 16);

    } else
       loadReg(execfile, 16, "0");
    
    //This ensures that if there is more than one item, they will all have the same value
    i = 0;
    while(i++ < nbytes)
        stackPush(execfile, 16);//copyRegToMem(execfile, valIdx + i++, 16);
            
}

void processIfElse(FILE* execfile, char* subline, int tokenid) {

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


}


void processWhileLoop(FILE* execfile, char* subline, int tokenid) {


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

void processPtr(FILE* execfile, char* subline, int tokenid) {
    
    //Creating variables in a loop might not be appreciated
    if(getLoopDepth() > 1) {
        char buffer[50 + strlen(subline)];
        sprintf(buffer, "Attempting to create ptr inside loop:\n%s\n", subline);
        throwWarning(buffer);
    }
    
    //Get the length of the string
    int len = 0;
    while(subline[len] && subline[len] != ' ' && subline[len] != '=') len++;

    char* varname = (char*) malloc((len+1) * sizeof(char));
    int j = -1;
    while(++j < len)
        varname[j] = subline[j];
    varname[len] = '\0';

    /* Adds the variable. A pointer is two bytes due to 16-bit architecture */
    addVariable(execfile, "ptr", varname, 2);

    int i = len;
    while(subline[i] == ' '); i++;

    if(subline[i] == '=') {
        while(subline[++i] == ' ');

        //There should be something here. If not, throw an error.
        if(!subline[i]) {
            char buffer[64 + strlen(subline)];
            sprintf(buffer, "Attempting to assign value to new ptr without providing value.\n%s\n", subline);
            throwError(buffer);
        } else if(subline[i] == '#') {
            char* tmp = contentToOperator(&subline[i+1], ' ', '[', ']');
            loadStackAddressOf(execfile, tmp); 
            free(tmp);

        } else {
            /* The value is a number */
            int addrs = atoi(&subline[i+1]);
            char buffer[32];
            sprintf(buffer, "ldi yh, %i\nldi yl, %i\n", addrs/256, addrs%256);
            writeAsmBlock(execfile, buffer);
        }


    } else {
        writeAsmBlock(execfile, "ldi yh, 0\nldi yl, 0\n");
    }

    //Put the value into the slot piece by piece.
    //I elected to push high, then low in case of arithmetic
    //between types of different sizes
    writeAsmBlock(execfile, "push yh\npush yl\n");

}

