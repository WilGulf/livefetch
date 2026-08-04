#include "config.h"

#include "ncurses.h"

#include <stdio.h>
#include <string.h>

#include "paths.h"
#include "sysinfo.h"
#include "util.h"
#include "globals.h"

char keys[16][64];
int keys_int = 0;
char values[16][64];

static int modules_array[32];

const char *get_logo(char *arg_logo, struct sysinfo *info) {
    static char logo[64];
    if (arg_logo != NULL) {
        strcpy(logo, arg_logo);
    } else {
        for (int i = 0; i < 16; i++) {
            if (strcmp(keys[i], "logo") == 0) {
                strcpy(logo, values[i]);
            }
        }
    }

    if (strcmp(logo, "default") == 0) {
        static char buffer[256];
    #ifdef __linux__
        snprintf(buffer, sizeof(buffer), "%s/%s.txt", get_logo_path(), info->os_id);
        if (!path_exists(buffer)) {
            snprintf(buffer, sizeof(buffer), "%s/linux_unknown.txt", get_logo_path());
        }
    #elif defined(__APPLE__)
        snprintf(buffer, sizeof(buffer), "%s/macos.txt", get_logo_path());
    #endif
        return buffer;
        
    if (strcmp(logo, "none") == 0) {
        return NULL;
    }
    } else {
        int j = 0;
        while (logo[j]) {
            if (logo[j] == '/') {
                return logo;
            }
            j++;
        }

        static char buffer[256];
        snprintf(buffer, sizeof(buffer), "%s/%s.txt", get_logo_path(), logo);
        return (const char *)buffer;
    }

    static char buffer[256];
#ifdef __linux__
    snprintf(buffer, sizeof(buffer), "%s/%s.txt", get_logo_path(), info->os_id);    
#elif defined(__APPLE__)
    snprintf(buffer, sizeof(buffer), "%s/macos.txt", get_logo_path());
#endif
    return buffer;
}

bool get_force_update() {
    for (int i = 0; i < 16; i++) {
        if (strcmp(keys[i], "force_update") == 0) {
            if (strncmp(values[i], "yes",3) == 0)
                return true;
            else if (strncmp(values[i], "true", 4) == 0)
                return true;
            else if (strncmp(values[i], "1", 1) == 0)
                return true;
            else
                return false;
        }
    }

    return false;
}

bool get_updating_visualizer() {
        for (int i = 0; i < 16; i++) {
            if (strcmp(keys[i], "updating_visualizer") == 0) {
                if (strncmp(values[i], "no", 2) == 0)
                    return false;
                else if (strncmp(values[i], "false", 5) == 0)
                    return false;
                else if (strncmp(values[i], "0", 1) == 0)
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
                        done = true;
                        break;
                    } else if (line[i] == '\n') {
                        modules_array[entries] = string_to_module_num(temp);
                        entries++;
                        done = true;
                        break;
                    } else if (line[i] == '}') {
                        is_table = false;
                        done = true;
                        break;
                    }else if (line[i] != ' ') {
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

        fclose(file);
    } else {
        printw("could not open config: %s", path);
        refresh();
        napms(10000);

        return 0;
    }

    return 1;
}