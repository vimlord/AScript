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
        
        pemdas(execfile, subline[i+1] ? &subline[i+1] : "0", 1);
        
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
   
    jumpIfFalse(execfile, condition, elseLabel, 1);
    
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
    jumpIfFalse(execfile, condition, endWhileLabel, 1);
    
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
    while(subline[i] == ' ') i++;
    
    if(subline[i] == '=') {
        while(subline[++i] == ' ');
        
        //There should be something here. If not, throw an error.
        if(!subline[i]) {
            throwError("Attempting to assign value to new ptr without providing value.\n%s\n", subline);
        } else if(subline[i] == '@') {
            char* tmp = contentToOperator(&subline[i+1], ' ', '[', ']');
            loadStackAddressOf(execfile, tmp); 
            free(tmp);

        } else {
            /* The value is a number */
            pemdas(execfile, subline[i] ? &subline[i] : "0", 2);
            return; //The returns to avoid redundant pop, only to repush
        }


    } else {
        writeAsmBlock(execfile, "ldi yh, 0\nldi yl, 0\n");
    }

    //Put the value into the slot piece by piece.
    //I elected to push high, then low in case of arithmetic
    //between types of different sizes
    writeAsmBlock(execfile, "push yh\npush yl\n");

}

void processByteAssign(FILE* execfile, char* line, char* varname, char* arrIdxStr) {
        char* tokidx = strstr(line, varname);

        //The variable is in the string

        int idx = (int) strlen(varname);

        //By default, no equals sign means that the value will be set to 0.
        while(tokidx[idx] && tokidx[idx] != '=') idx++;
        writeComment(execfile, "Computing value"); 
        pemdas(execfile, tokidx[idx+1] ? &tokidx[idx+1] : "0", 1); //Compute the value
        writeComment(execfile, "Determining pointer address");
        
        //The access is done as an array
        //First, calculate the address
        char addrBuffer[64];

        /*
         * We will need the zero index of the stack
         * (x always holds index 0 of the stack)
         * It will be copied to y
         */
        writeAsmBlock(execfile, "mov yh, xh\nmov yl, xl\n");
        //Gets index on stack
        int stkIdx = stackAddressOfVar(varname);
        if(stkIdx < 0) {
            //The variable does not exist on the stack.
            throwError("A variable was not found.\n");
        }

        /**
         * The index of the variable on the stack is then subtracted
         * from the pointer to the bottom of the stack.
         */
        sprintf(addrBuffer, "ldi r16, %i\nldi r17, %i\n", stkIdx % 256, stkIdx / 256);
        writeAsmBlock(execfile, addrBuffer);
        writeAsmBlock(execfile, "sub yl, r16\nsbc yh, r17\n");

        //Increments y by the array address.
        if(arrIdxStr) {
            writeComment(execfile, "Computing array index");
            //Then, we calculate the index
            pemdas(execfile, arrIdxStr, 2);
            //Next, we pop the value off of the stack
            stackPop(execfile, 16);
            stackPop(execfile, 17);

            //Then, we need to add it to the zero memory address
            writeAsmBlock(execfile, "add yl, r16\nadc yh, r17");
        } 
        
        writeComment(execfile, "Storing end result");

        //Finally, we get the variable and put it in the slot
        stackPop(execfile, 0x10);
        sprintf(addrBuffer, "st y, r16\n");
        writeAsmBlock(execfile, addrBuffer);
            
}


void processPtrAssign(FILE* execfile, char* line, char* varname, char* arrIdxStr) {
    char* subline = strstr(line, varname);
    while(*subline && *subline != '=') subline = &subline[1];
    
    if(!(*subline)) {
        throwError("Pointer without assignment.\n%s\n", strstr(line, varname));
    }

    int i = 0;
    while(subline[++i] == ' ');
    
    /* In any case, the end result will be that the result is in y */
    if(subline[i] == '@') {
        //Get address of variable
        char* tmp = contentToOperator(&subline[i+1], ' ', '[', ']');
        loadStackAddressOf(execfile, tmp);
    } else if(subline[i] == '#') {
        //Get contents of address
        int val = atoi(&subline[i+1]);
        char buffer[64];
        sprintf(buffer, "ldi zh, %i\nldi zl, %i\n", val/256, val%256);
        writeAsmBlock(execfile, buffer);
        //Put the result into y
        writeAsmBlock(execfile, "ld yl, z+\nld yh, z+\n");
    } else {
        //Determine the value of the equality (there will be a two byte val)
        pemdas(execfile, subline[i] ? &subline[1] : "0", 2);
        //Pops the result into y
        writeAsmBlock(execfile, "pop yl\npop yh\n");
    }
    
    //Get the assignment address
    int stkptr = stackAddressOfVar(varname);
    writeAsmBlock(execfile, "mov zh, xh\nmov zl, xl\n");

    char buffer[64];
    sprintf(buffer, "ldi r16, %i\nsub zl, r16", stkptr % 256);
    writeAsmBlock(execfile, buffer);

    sprintf(buffer, "ldi r16, %i\nsbc zl, r16", stkptr / 256);
    writeAsmBlock(execfile, buffer);

    //Perform assignment
    writeAsmBlock(execfile, "st z+, yl\nst z, yh\n");

}

void processFunction(FILE* execfile, char* subline, int tokenid) {
    if(getLoopDepth() != 0) {
        char buffer[50 + strlen(subline)];
        sprintf(buffer, "Attempting to create ptr inside loop:\n%s\n", subline);
        throwWarning(buffer);
    }
    
    int len = 0;
    while(subline[len] != ' ' && subline[len]) len++;

    if(!subline[len]) {
        throwError("Function must have return type.\nfunction %s\n", subline);
    }
    
    //The return type
    CMP_TOK returnType = (char*) malloc((len+1) * sizeof(char));
    int i = 0;
    while(i < len) {
        returnType[i] = subline[i];
        i++;
    }
    returnType[len] = '\0';
    
    char* func = &subline[len+1];
    while(*func == ' ') func = &func[1];

    len = 0;
    while(func[len] != ' ' && func[len]) len++;

    if(!subline[len]) {
        throwError("Function must have name.\nfunction %s\n", subline);
    }
    
    //The function name
    char* functionName = (char*) malloc((len+1) * sizeof(char));
    i = 0;
    while(i < len) {
        functionName[i] = func[i];
        i++;
    }
    functionName[len] = '\0';

    char* par = &func[len+1];
    while(subline[len] != ' ' && subline[len]) len++;
    
    if(subline[len] != '(') {
        throwError("Function name should be followed by arguments in parentheses\nfunction %s\n", subline);
    }
    
    //The parameter list
    char* params = closureContent(&par[1], '(', ')');

    len = 1 + strlen(params);
    while(par[len] != '{') len++;

    //The code inside the function
    char* codeBlock = closureContent(&par[len+1], '(', ')');
    
    //Writes the function label
    writeAsmBlock(execfile, "function_"); writeAsmBlock(execfile, functionName); writeAsmBlock(execfile, "\n");
    
    //Get the number of arguments
    i = 0;
    int parCount = 0;
    while(params[i]) {
        if(!parCount && params[i] != ' ')
            parCount++;
        if(params[i] == ',')
            parCount++;
        i++;
    }

    if(compTok("void", returnType)) {
        setCompilerStackTop(-2*(parCount+3));
        addVariable(execfile, returnType, "return", 2);
    } else setCompilerStackTop(-2*(parCount+2));
    
    //All of the parameters should be in place. So, claim the spaces.
    i = 0;
    while(i < parCount) {
        int j = 0;
        while(params[j] == ' ') j++;
        char* param = contentToOperator(&params[j], ',', ' ', ' ');
        char* tp = contentToOperator(param, ' ', ' ', ' ');
        char* nm = contentToOperator(&param[strlen(tp) + 1], '\0', '\0', '\0');
        addVariable(execfile, tp, nm, 2);

        free(param);
    }

    //Executes the code
    parseSegment(execfile, codeBlock);
    
    //Return to the previous point of operation in the assembly code
    writeAsmBlock(execfile, "ret\n");

}


void handleReturn(FILE* execfile, char* subline, int tokenid) {
    //The return space is always two bytes, so perform
    //two byte computations. Puts the return value into the
    //return slot, which will be on the top of the stack
    //after returning from the function.
    processPtrAssign(execfile, subline, "return", "0");
    
    //Change the stack pointer to point back to the beginning
    int size = ((VarFrame)getFromList(getVars(), listSize(getVars()) - 1))->addr + 1;
    writeAsmBlock(execfile, "in r16, spl\nin r17,sph\n");
    
    char buff[256];
    sprintf(buff, "ldi r18, %i\nldi r19, %i\n", size%256, size/256);
    writeAsmBlock(execfile, buff);
    writeAsmBlock(execfile, "add r16, r18\nadc r17, r19\n");
    writeAsmBlock(execfile, "out spl, r16\nout sph, r17\n");

    //Performs the return.
    writeAsmBlock(execfile, "ret\n");
    

}




