#include "main.h"
#include "srccompile.h"
#include "strmanip.h"
#include "asmcommands.h"

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
                printf("Error during compilation: No output file provided.\n");
                return EINVAL;
            } else
                OUTPUTNAME = argv[i];
        } else
            //The value must be an output
            INPUTNAME = argv[i];
        

        i++;
    }

    if(!INPUTNAME) {
        //No input file.
        printf("Error during compilation: No input file specified.\n");
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
    
    if(strcmp(inputext, ".scr")) {
        printf("Error during compilation: Input file extension must be .scr\n");
        return EINVAL;
    } else if(strcmp(outputext, ".asm")) {
        printf("Error during compilation: Output file extension must be .asm\n");
        return EINVAL;
    }



    //The source file
    FILE* sourceFile = fopen(INPUTNAME, "r");
    
    //Checks to see if the file exists, and returns an error
    //if the file does not exist.
    if(sourceFile) {
        printf("Compiling %s...\n", INPUTNAME);
    } else {
        printf("Error during compilation: Input file not found.\n");
        return ENOENT;
    }

    //Create a swap file for writing
    printf("Opening swapspace0...\n");
    FILE* swapFile0 = fopen(".swapspace0.dta", "w+");
    
    //Moves the chars to the new file, without the newlines.
    printf("Morphing source into swapspace0...\n");
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

    
    printf("Closing source and swapspace0...\n");
    //At this point, .swapspace0.swp contains 
    //the source without newlines.
    fclose(sourceFile); //Source file is no longer needed
    fclose(swapFile0);
    
    printf("Opening swapspace0 for reading...\n");

    //Reopen the swap file, only this time for reading.
    swapFile0 = fopen(".swapspace0.dta", "r");

    printf("Opening asm file for writing...\n");

    //Create new stack frame file and execution file.
    //Hold stack frame data and execution instructions.
    FILE* execdata = fopen(OUTPUTNAME, "w");
    
    //Default execution data
    writeComment(execdata, "Sets initial system values");
    writeAsmBlock(execdata, "ldi sph, high(RAMEND)\n");
    writeAsmBlock(execdata, "ldi spl, low(RAMEND)\n");
    writeAsmBlock(execdata, "mov xh, sph\n");
    writeAsmBlock(execdata, "mov xl, spl\n");

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

    if(*nextLine == EOF)
        printf("Finalizing...\n");
    else
        printf("Something went wrong: %c\n", *nextLine);

    fclose(swapFile0);

    //Final cleanup
    remove(".swapspace0.dta");

    fclose(execdata);

}

