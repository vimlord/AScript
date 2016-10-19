#include "main.h"
#include "srccompile.h"

#include <errno.h>
#include <string.h>

char* getCodeBlock(FILE* file) {
    return stringUpTo(file, '}', '{', '}');
}

char* getNextLine(FILE* file) {
    return stringUpTo(file, ';', '{', '}');
}

char* stringUpTo(FILE* file, char c, char up, char down) {

    int i = 0, j = 0;
    char* line = (char*) malloc(sizeof(char)); //Creates empty string.
    char* swp;
    char nextChar;
    
    int level = 0; //Closure level. Closure can be reached when level == 0

    *line = fgetc(file);

    if(*line == EOF) return line;
    else while(*line == ' ') *line = fgetc(file);
    
    /**
     * Iterates until the end of the file or until the terminator
     * has been reached, provided that closure has been reached.
     * A compilable program will have closure on every line.
     */
    while(line[i] != EOF && (line[i] != c || level)) {
        i++;
        //Read char
        nextChar = fgetc(file);

        if(nextChar == up)
            level++;
        if(nextChar == down)
            level--;

        //Create new memory slot, and put line into it
        swp = (char*) malloc(i+1 * sizeof(char));
        j = 0;
        while(j < i) {
            swp[j] = line[j];
            j++;
        }
        swp[i] = nextChar;
        
        //Reallocate memory and do freeing. 
        if(line) free(line);
        line = swp;
    }

    line[i] = '\0'; //Terminates the string.
    return line;

}

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

