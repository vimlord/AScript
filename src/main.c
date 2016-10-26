#include "main.h"

#include "asmcommands.h"
#include "srccompile.h"
#include "strmanip.h"
#include "optimization.h"
#include "error.h"

#include <errno.h>
#include <string.h>

int main(int argc, char* argv[]) {

    //If no file is provided, immediately exit w/ an error message.
    
    char* INPUTNAME = NULL;
    char* OUTPUTNAME = NULL;

    //Parse through user arguments
    int i = 1;
    while(i < argc) {
        
        if(strcmp(argv[i], "-o") == 0) {
            //The user wishes to specify an output
            if(!argv[++i]) {
                throwError("No output file specified.");
                return EINVAL;
            } else
                OUTPUTNAME = argv[i];
        } else if(*(argv[i]) == '-') {
            int j = 0;
            while(argv[i][j++]) {
                if(argv[i][j] == 'O')
                    addErrorFlags(2);
                else if(argv[i][j] == 'w')
                    addErrorFlags(1);
            }
        } else
            //The value must be an output
            INPUTNAME = argv[i];
        

        i++;
    }

    if(!INPUTNAME) {
        //No input file.
        throwError("No input file specified.");
        return EINVAL;
    } else if(!OUTPUTNAME) {
        //Gets the name of the output file, since none is specified.
        i = 0;
        while(INPUTNAME[i++] != '.');
        //Make space and assign values
        OUTPUTNAME = malloc((i+4) * sizeof(char));
        i = 0;
        while((OUTPUTNAME[i] = INPUTNAME[i]) != '.') i++;
        //Sticks the file extension at the end
        int j = 0;
        for(j = 0; j <= 3; j++)
            OUTPUTNAME[i+j+1] = "asm"[j];
    }
    
    //Gets the file extensions for error checking
    char* inputext = INPUTNAME;
    char* outputext = OUTPUTNAME;
    while(*inputext && *inputext != '.')
        inputext = &inputext[1];
    while(*outputext && *outputext != '.')
        outputext = &outputext[1];
    
    if(strcmp(inputext, ".scr"))
        throwError("Input file extension must be .scr");
    else if(strcmp(outputext, ".asm"))
        throwError("Output file extension must be .asm");
    

    //The source file
    FILE* sourceFile = fopen(INPUTNAME, "r");
    
    //Checks to see if the file exists, and returns an error
    //if the file does not exist.
    if(!sourceFile)
        throwError("Input file not found.");

    //Create a swap file for writing
    FILE* swapFile0 = fopen(".swapspace0.dta", "w+");
    
    //Moves the chars to the new file, without the newlines.
    char* nextLine;
    while(*(nextLine = stringUpTo(sourceFile, '\n', '\0', '\0')) != EOF) {
        //Gets the length of the String.
        int i = -1;
        while(nextLine[++i]);
        
        //printf("%s\n", nextLine);
        fwrite(nextLine, 1, i, swapFile0);

        //Needs to be freed
        free(nextLine);
    }

    
    //At this point, .swapspace0.swp contains 
    //the source without newlines.
    fclose(sourceFile); //Source file is no longer needed
    fclose(swapFile0);

    //Reopen the swap file, only this time for reading.
    swapFile0 = fopen(".swapspace0.dta", "r");

    //Create new stack frame file and execution file.
    //Hold stack frame data and execution instructions.
    FILE* execdata = fopen(getErrorFlag(1) ? ".asm.dta" : OUTPUTNAME, "w");
    
    //Default execution data
    writeComment(execdata, "Sets initial system values");
    writeAsmBlock(execdata, "ldi xh, high(RAMEND)\n");
    writeAsmBlock(execdata, "out sph, xh\n");
    writeAsmBlock(execdata, "ldi xl, low(RAMEND)\n");
    writeAsmBlock(execdata, "out spl, xl\n");

    writeAsmBlock(execdata, "\n");

    /*
     * Iterates through each line one by one. The use of parseSegment()
     * allows for lines containing code segments to be broken down.
     */
    nextLine = getNextLine(swapFile0);
    while(*nextLine != '\0' && *nextLine != EOF) {

        //Parse the current line.
        parseSegment(execdata, nextLine);
        
        //Gets the next line
        nextLine = getNextLine(swapFile0);
    }

    //Closes the two swap files
    fclose(swapFile0);
    fclose(execdata);
    
    if(getErrorFlag(1)) {

        FILE* prepped = fopen(".asm.dta", "r");
        FILE* result = fopen(OUTPUTNAME, "w");
    
        performOptimizations(prepped, result);
    
        fclose(result);
    } else
        remove(".swapspace0.dta");

    remove(".asm.dta");
    
    printf("%sCompilation successful.%s\n", "\033[1m\033[32m", "\x1B[0m");
    printf("%sFile created at %s\n", "\x1b[0m", OUTPUTNAME);

}

