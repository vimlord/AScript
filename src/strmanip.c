#include "strmanip.h"

#include "srccompile.h"

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

int indexOfClosingChar(char* str, char start, char end) {
    int len = 0, i = 0;
    
    while(str[len]) {
        if(str[len] == start)
            i++;
        else if(str[len] == end) {
            if(i)
                i--;
            else
                break;
        }
        len++;
    }
    
    return len;
}

char* closureContent(char* str, char start, char end) {
    int len = 0, i = 0;
    while(str[len]) {
        if(str[len] == start)
            i++;
        else if(str[len] == end) {
            if(i)
                i--;
            else
                break;
        }
        len++;
    }
    
    //The inveriant is that len is at the end of the content
    char* result = (char*) malloc((len+1) * sizeof(char));
    i = 0;
    while(i < len) {
        result[i] = str[i];
        i++;
    }
    result[i] = '\0';

    return result;

}

char* parenthesesContent(char* start) {
    return closureContent(start, '(', ')');
}

char* bracketContent(char* start) {
    return closureContent(start, '{', '}');
}

char* contentToOperator(char* start, char op, char up, char down) {
    int len = 0;
    
    while(start[len] != '\0') {
        if(start[len] == op)
            break;
        else if(start[len] == up) {
            len += indexOfClosingChar(&start[len+1], up, down) + 1;
        }

        //printf("PARSE: '%s'\n", &start[len]);
        len++;

    }
    
    char* result = (char*) malloc((len + 1) * sizeof(char));
    int i = -1;
    while(++i < len)
        result[i] = start[i];
    result[i] = '\0';

    return result;
}





