#ifndef _STRMANIP_H_
#define _STRMANIP_H_

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

/**
 * Gets a section of the file up until the first instance of
 * a given character that allows for closure.
 *
 * file - The file to read from.
 * c    - The terminating character.
 * up   - The character of positive closure.
 * down - The character of negative closure.
 *
 * NOTE - The number of occurences of up will be equal to the
 *        number of occurences of down unless EOF is reached first.
 * NOTE - A string S with closure is a string such that
 *            a) The number of characters of positive closure is equal to
 *               the number of characters of negative closure
 *            b) For any n <= len(S), the string T consisting of the first
 *               n characters of S has at least as many positve closure
 *               characters as negative closure characters.
 */
char* stringUpTo(FILE* file, char c, char up, char down);

/**
 * Gets a section of the string up until the first instance of
 * a given character that allows for closure.
 *
 * str  - The string to read from.
 * c    - The terminating character.
 * up   - The character of positive closure.
 * down - The character of negative closure.
 *
 * NOTE - The number of occurences of up will be equal to the
 *        number of occurences of down unless EOF is reached first.
 * NOTE - A string S with closure is a string such that
 *            a) The number of characters of positive closure is equal to
 *               the number of characters of negative closure
 *            b) For any n <= len(S), the string T consisting of the first
 *               n characters of S has at least as many positve closure
 *               characters as negative closure characters.
 */
int indexOfClosingChar(char* str, char start, char end);

/**
 * Gets the shortest string that allows closure.
 * str   - The string starting at the char after a
 *         char equal to start (str[-1] == start).
 * start - An opening character
 * end   - A closing character
 */
char* closureContent(char* str, char start, char end);

char* parenthesesContent(char* start);
char* bracketContent(char* start);

/**
 * Gets everything up until a certain operator, as long as there
 * is closure in the string as definied by the up and down characters
 */
char* contentToOperator(char* start, char op, char up, char down);



#endif


