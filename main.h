#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>


/*
 * A code block is the content btwn two curly braces { and }
 * Uses read(), so is expected to skip over chars.
 * Returns a null-terminated string consisting
 * of the code block without the closing bracket.
 */
char* getCodeBlock(FILE* file);

/*
 * A line is the content btwn two semicolons
 * Uses read(), so is expected to skip over chars.
 * Returns a null-terminated string consisting
 * of the next line without the semicolon.
 */
char* getNextLine(FILE* file);

char* stringUpTo(FILE* file, char c);
