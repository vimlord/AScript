#include "optimization.h"

#include "asmcommands.h"
#include "error.h"
#include "strmanip.h"

#include <string.h>

int optimizeAsm(FILE* src, FILE* dst) {
    char* prevLine = NULL; //The previous line
    char* currLine = NULL; //The current line
    
    char buffer[256];

    int i, j;
    int changes = 0;
    
    /* Performs optimization by removing redundant lines one at a time */
    while(*(currLine = stringUpTo(src, '\n', '\0', '\0')) != EOF) {
        
        if(*currLine == ';' || *currLine == '\0') {
            //Comment and blank lines will be ignored
            free(currLine);
            continue;
        }
        
        //printf("%s\n", currLine);

        if(prevLine) {
            if (strstr(prevLine, "push ") == prevLine &&
                strstr(currLine, "pop ") == currLine) {
                /*
                Push-Pop Redundancy (push x; pop x)
                Saves 3 clock cycles per change
                3+3 before, 1 after
                */
                changes++;
                
                //printf("Merging '%s' and '%s' into ", prevLine, currLine);
                
                sprintf(buffer, "mov %s, %s", &currLine[4], &prevLine[5]);
                i = 0;
                while((prevLine[i] = buffer[i])) i++;

                //printf("'%s'\n", prevLine);

                //Makes the line unreadable
                free(currLine);
                continue;
            } else if(strstr(currLine, "mov ") == currLine) {
                //Opportunity for redundant assignments exists

                //Gets the index of the char after the comma
                i = 4;
                while(currLine[i++] != ',');
                j = 4;
                while(currLine[i] && currLine[j++] == currLine[i++]);

                if(!currLine[i]) {
                    /*
                    MOV Redundancy (mov x, x)
                    Saves 1 clock cycle per change
                    1 before, 0 after
                    */
                    changes++;
                    free(currLine);
                    continue;
                }

            } else if (!strcmp(prevLine, currLine) && 
                        (strstr(currLine, "ldi ") == currLine ||
                         strstr(currLine, "mov ") == currLine)) {
                /*
                Extra Action (ldi x, y; ldi x, y) or (mov x, y; mov x, y)
                Saves 1 clock cycle per change
                1 before, 0 after
                */
                changes++;
                free(currLine);
                continue;
            }
            
            if(*prevLine) { 
                writeAsmBlock(dst, prevLine);
                writeAsmBlock(dst, "\n");
            }

            free(prevLine);
        }

        prevLine = currLine;
    }
    
    //Write the last remaining block, if one exists
    if(prevLine)
        writeAsmBlock(dst, prevLine);
    
    //Free memory
    free(prevLine);
    free(currLine);

    return changes;

}

/**
 * Performs optimization on assembly code and puts the end
 * result into the target file.
 *
 * src - The source file, opened for reading.
 * dst - The destination file, opened for writing.
 */
void performOptimizations(FILE* src, FILE* dst) {
    FILE* input = NULL;
    FILE* output = fopen(".swapspace0.dta", "w");
    
    int i = 0;
    int count = optimizeAsm(src, output);
    char filename[16];

    int totalChanges = count;

    fclose(output);

    while(count) {
        sprintf(filename, ".swapspace%i.dta", i+1);
        output = fopen(filename, "w");

        sprintf(filename, ".swapspace%i.dta", i++);
        input = fopen(filename, "r");

        totalChanges += (count = optimizeAsm(input, output));
        
        fclose(input);
        remove(filename);
        fclose(output);
    }

    sprintf(filename, ".swapspace%i.dta", i);
    input = fopen(filename, "r");
    char c;
    while((c = fgetc(input)) != EOF)
        fputc(c, dst);
    
    fclose(input);
    remove(filename);

    printf("\033[1m\033[34m" "Made %i optimizations in %i passes." "\x1B[0m", totalChanges, i+1);
    printf("\n\n");
}

