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

bool string_is_statement(bool statement, const char *str) {
    if (statement) {
         if (strncmp(str, "yes", 3) == 0)
        return true;
    else if (strncmp(str, "true", 4) == 0)
        return true;
    else if (strncmp(str, "1", 1) == 0)
        return true;
    else
        return false;
    } else {
        if (strncmp(str, "no", 2) == 0)
            return true;
        else if (strncmp(str, "false", 5) == 0)
            return true;
        else if (strncmp(str, "0", 1) == 0)
            return true;
        else
            return false;
    }
}
