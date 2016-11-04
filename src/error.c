#include "error.h"

#include <stdarg.h>

#include <stdio.h>
#include <stdlib.h>

int ERROR_FLAGS;

void addErrorFlags(int flags) {
    ERROR_FLAGS |= flags;
}

void remErrorFlags(int flags) {
    ERROR_FLAGS &= ~flags;
}

int getErrorFlags() {
    return ERROR_FLAGS;
}

int getErrorFlag(int i) {
    return (ERROR_FLAGS >> i) & 1;
}

void throwError(char* mssg, ...) {
    printf("%sERROR: %s", "\033[1m\033[31m", "\x1B[0m");

    va_list args;

    va_start(args, mssg);
    vprintf(mssg, args);
    va_end(args);

    printf("\n");
    exit(1);
}

void throwWarning(char* mssg, ...) {
    printf("%sWARNING: %s", "\033[1m\033[33m", "\x1B[0m");

    va_list args;

    va_start(args, mssg);
    vprintf(mssg, args);
    va_end(args);

    if(getErrorFlag(0))
        exit(1);
}

void throwDebug(char* mssg, ...) {
    printf("%sDEBUG: %s", "\033[1m\033[36m", "\x1B[0m");

    va_list args;

    va_start(args, mssg);
    vprintf(mssg, args);
    va_end(args);

    printf("\n");
}
