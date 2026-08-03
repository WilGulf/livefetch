#include <stdbool.h>
#include <sys/stat.h>
#include <string.h>

#include "util.h"

bool path_exists(const char *filename) {
    struct stat buffer;
    return stat(filename, &buffer) == 0 ? true : false;
}

void clean_string(char *buffer) {
    int i = 0;
    while (buffer[i]) {
        if (buffer[i] == '"' || buffer[i] == '\n' || buffer[i] == '=') {
            memmove(&buffer[i], &buffer[i + 1], strlen(&buffer[i + 1]) + 1);
            i--;
        }
        i++;
    }
}

