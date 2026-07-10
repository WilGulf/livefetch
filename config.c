#include "ncurses.h"

#include "stdio.h"
#include "string.h"

char keys[16][64];
int keys_int = 0;
char values[16][64];

static int modules_array[32];

const char* get_logo() {
    for (int i = 0; i < 16; i++) {
        if (strcmp(keys[i], "logo") == 0) {
            if (strcmp(values[i], "default") == 0) {
                return "logos/macos2.txt";
            } else {
                int j = 0;
                while (values[i][j]) {
                    if (values[i][j] == '/') {
                        return values[i];
                    }
                    j++;
                }
                static char buffer[64];
                snprintf(buffer, 64, "logos/%s.txt", values[i]);
                return (const char *)buffer;
            }
        }
    }

    return "logos/macos2.txt";
}

bool get_updating_visualizer() {
        for (int i = 0; i < 16; i++) {
            if (strcmp(keys[i], "updating_visualizer") == 0) {
                if (strcmp(values[i], "no") == 0)
                    return false;
                else if (strcmp(values[i], "false") == 0)
                    return false;
                else if (strcmp(values[i], "0") == 0)
                    return false;
                else
                    return true;
        }
    }

    return true;
}

int string_to_module_num(const char *string) {
    if (strcmp(string, "hostname") == 0)
        return 1;
    else if (strcmp(string, "os") == 0)
        return 2;
    else if (strcmp(string, "kernel") == 0)
        return 3;
    else if (strcmp(string, "uptime") == 0)
        return 4;
    else if (strcmp(string, "packages") == 0)
        return 5;
    else if (strcmp(string, "shell") == 0)
        return 6;
    else if (strcmp(string, "display") == 0)
        return 7;
    else if (strcmp(string, "terminal") == 0)
        return 8;
    else if (strcmp(string, "cpu") == 0)
        return 9;
    else if (strcmp(string, "gpu") == 0)
        return 10;
    else if (strcmp(string, "memory") == 0)
        return 11;
    else if (strcmp(string, "swap") == 0)
        return 12;
    else if (strcmp(string, "disk") == 0)
        return 13;
    else if (strcmp(string, "local_ip") == 0)
        return 14;
    else if (strcmp(string, "battery") == 0)
        return 15;
    else if (strcmp(string, "locale") == 0)
        return 16;
    else if (strcmp(string, "spacer") == 0)
        return 99;
    else
        return 0;
}

int get_module(int num) {
    return modules_array[num];
}

int parse_config(const char *path) {
    bool is_table = false;

    FILE* file = fopen(path, "r");

    char line[256];
    if (file != NULL) {
        int entries = 0;

        bool is_table = false;

        while (fgets(line, sizeof(line), file)) {
            int i = 0;
            bool done = false;

            if (is_table) {
                char temp[256] = "";
                int j = 0;
                while (line[i]) {
                    if (line[i] == '#') {
                        modules_array[entries] = string_to_module_num(temp);
                        entries++;
                        done = true;
                        break;
                    } else if (line[i] == '\n') {
                        modules_array[entries] = string_to_module_num(temp);
                        entries++;
                        done = true;
                        break;
                    } else if (line[i] != ' ') {
                        temp[j] = line[i];
                        j++;
                    }
                    
                    i++;
                }
                
            } else {
                while (line[i] && !done) {
                    if (line[i] == '#') {
                        break;
                    } else if (line[i] == '=' || is_table) {
                        int j = 0;
                        for (; j < i; j++) {
                            keys[keys_int][j] = line[j];
                        }

                        if (strcmp(keys[keys_int], "modules") == 0 && line[j + 1] == '{') {
                            is_table = true;
                            done = true;
                            break;
                        } else {
                            j++;

                            int k = 0;
                            while (line[j] && line[j] != '#' && line[j] != '\n') {
                                values[keys_int][k] = line[j];
                                k++;
                                j++;
                            }

                            keys_int++;
                            done = true;
                        }
                    }

                    i++;
                }
                
                done = false;
            }
        }
    } else {
        printw("could not open config: %s", path);
        refresh();
        napms(1000);

        return 0;
    }

    return 1;
}