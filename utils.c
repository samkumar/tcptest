#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

void error_exit(const char* message) {
    perror(message);
    exit(1);
}

void retval_exit(int retval, const char* message) {
    if (retval == -1) {
        fprintf(stderr, "%s: malloc failed\n", message);
        exit(1);
    } else {
        errno = retval;
        error_exit(message);
    }
}
