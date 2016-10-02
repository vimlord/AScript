#include "main.h"
#include "srccompile.h"

#include <errno.h>

char* getCodeBlock(FILE* file) {
    return stringUpTo(file, '}');
}

char* getNextLine(FILE* file) {
    return stringUpTo(file, ';');
}

char* stringUpTo(FILE* file, char c) {

    int i = 0, j = 0;
    char* line = (char*) malloc(sizeof(char));;
    char* swp;
    char nextChar;
    
    *line = fgetc(file);
    

    if(*line == EOF) return line;
    else while(*line == ' ') *line = fgetc(file);

    while(line[i] != c) {
        i++;
        //Read char
        nextChar = fgetc(file);

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

    line[i] = '\0';
    
    return line;

}

int main(int argc, char* argv[]) {
    
    //If no file is provided, immediately exit w/ an error message.
    if(argc < 2) {
        printf("Error during compilation: No input file specified.\n");
        return EINVAL;
    }
    
    //The name of the file
    char* FILENAME = argv[1];
    
    //The source file
    FILE* sourceFile = fopen(FILENAME, "r");
    
    //Checks to see if the file exists, and returns an error
    //if the file does not exist.
    if(sourceFile)
        printf("Compiling %s...\n", FILENAME);
    else {
        printf("Error during compilation: Input file not found.\n");
        return ENOENT;
    }

    //Create a swap file for writing
    printf("Opening swapspace0...\n");
    FILE* swapFile0 = fopen(".swapspace0.dta", "w+");
    
    //Moves the chars to the new file, without the newlines.
    printf("Morphing source into swapspace0...\n");
    char* nextLine;
    while(*(nextLine = stringUpTo(sourceFile, '\n')) != EOF) {
        //Gets the length of the String.
        int i = -1;
        while(nextLine[++i]);
        
        printf("%s\n", nextLine);
        fwrite(nextLine, 1, i, swapFile0);

        //Needs to be freed
        free(nextLine);
    }

    
    printf("\nClosing source and swapspace0...\n");
    //At this point, .swapspace0.swp contains 
    //the source without newlines.
    fclose(sourceFile); //Source file is no longer needed
    fclose(swapFile0);
    
    printf("Opening swapspace0 for reading...\n");

    //Reopen the swap file, only this time for reading.
    swapFile0 = fopen(".swapspace0.dta", "r");

    printf("Opening dta files for writing...\n");

    //Create new stack frame file and execution file.
    //Hold stack frame data and execution instructions.
    FILE* stkdata = fopen(".stkdata.dta", "w");
    FILE* execdata = fopen(".execdata.dta", "w");
    
    int i = 0;
    nextLine = getNextLine(swapFile0);
    while(*nextLine != '\0' && *nextLine != EOF) {
        if(i++ > 10) break;
        //else printf("[%i]", EOF == *nextLine);
        //printf("%i\n", (int) *nextLine);

        //Parse the current line.
        parseLine(stkdata, execdata, nextLine);

        nextLine = getNextLine(swapFile0);
    }

    //Final cleanup
    remove(".swapspace0.dta");



}

