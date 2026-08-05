#ifndef CONFIG_H
#define CONFIG_H

#include <stdbool.h>
#include "sysinfo.h"

int parse_config(const char *path);
const char *get_logo(char *arg_logo, struct sysinfo *info);
bool get_value_bool(char *key);
int get_module(int num);

#endif