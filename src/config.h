#ifndef CONFIG_H
#define CONFIG_H

#include <stdbool.h>

int parse_config(const char *path);
const char *get_logo(char *arg_logo);
bool get_updating_visualizer();
int get_module(int num);

#endif