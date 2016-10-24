#include "error.h"

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

void throwError(char* mssg) {
    printf("%sERROR:%s %s\n", "\033[1m\033[31m", "\x1B[0m", mssg);
    exit(1);
}

void throwWarning(char* mssg) {
    printf("%sWARNING:%s %s\n", "\033[1m\033[33m", "\x1B[0m", mssg);
    if(getErrorFlag(0))
        exit(1);
}

void throwDebug(char* mssg) {
    printf("%sDEBUG:%s %s\n", "\033[1m\033[36m", "\x1B[0m", mssg);
}
