#include "error.h"

#include <stdio.h>
#include <stdlib.h>

void throwError(char* mssg) {
    printf("%sERROR:%s %s\n", "\033[1m\033[31m", "\x1B[0m", mssg);
    exit(1);
}

