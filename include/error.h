#ifndef _ERROR_H_
#define _ERROR_H_

/*
 * The error flags are stored in a 32-bit integer created by src/error.c. In the
 * data field, the following bits are assigned to the given checks:
 *
 * 0 - Exit on warning
 */

/**
 * Add or remove error checking flags.
 *
 * flags - The error messages to enable or disable.
 */
void addErrorFlags(int flags);
void remErrorFlags(int flags);

/**
 * Gets the data of the error flags.
 */
int getErrorFlags();

/**
 * Gets the error flag at an index via bitshift of i.
 */
int getErrorFlag(int i);

/**
 * Throws an error; prints the given message and terminates compilation.
 *
 * mssg - The error message to print.
 */
void throwError(char* mssg);

/**
 * Throws a warning; prints the given message as advice to the user.
 *
 * mssg - The warning to give.
 */
void throwWarning(char* mssg);

/**
 * Throws a debug message; prints the given message with a warning.
 *
 * mssg - The message to display.
 */
void throwDebug(char* mssg);

#endif


