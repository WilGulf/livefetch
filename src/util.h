#include <stdbool.h>

#ifndef UTIL_H
#define UTIL_H

bool path_exists(const char *filename);
void clean_string(char *buffer);
bool string_is_statement(bool statement, const char *str);

#endif