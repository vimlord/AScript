#include <stdio.h>

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

/**
 * Writes the assembly code that performs a PEMDAS operation based
 * on the C-style order of operations.
 *
 * execfile - The destination for the code to be written to.
 * calc     - A segment of code that contains the formula, which should have closure.
 * dst      - The address in memory where the result will be stored.
 */
void pemdas(FILE* execfile, char* calc);

/**
 * Jumps to a label if the value of the conditional is true.
 *
 * cond  - A pemdas parseable statement.
 * label - The label that will be jumped to on meeting the condition.
 * ptr   - The address where the result of computing cond can safely be stored.
 */
void jumpIfTrue(FILE* execfile, char* cond, char* label);

/**
 * Jumps to a label if the value of the conditional is false.
 *
 * cond  - A pemdas parseable statement.
 * label - The label that will be jumped to on meeting the condition.
 * ptr   - The address where the result of computing cond can safely be stored.
 */
void jumpIfFalse(FILE* execfile, char* cond, char* label);
