#include "ncurses.h"
#include "panel.h"

#include "string.h"
#include "unistd.h"

#include "config.h"
#include "sysinfo.h"

struct sysinfo system_info;

int modules_array[32];

void init_modules() {

}

const char* module(int num, bool is_updating) {
    if (is_updating) {
        switch (modules_array[num]) {
            case 1: {
                strcpy(system_info.hostname, getHostname());
                return system_info.hostname;
            }
            case 2: {
                strcpy(system_info.os, "macos tahoe");
                return system_info.os;
            }
            case 3: {
                strcpy(system_info.kernel, "darwin");
                return system_info.kernel;
            }
            case 4: {
                system_info.uptime = 1;
                static char formatted[64];
                snprintf(formatted, sizeof(formatted), "Uptime: %d", system_info.uptime);
                return formatted;
            }
            case 5: {
                system_info.package = 6767;
                static char formatted[64];
                snprintf(formatted, sizeof(formatted), "Packages Installed: %d", system_info.package);
                return formatted;
            }
            case 6: {
                strcpy(system_info.shell, "bash");
                return system_info.shell;
            }
            case 7: {
                strcpy(system_info.display, "4K @ 240hz");
                return system_info.display;
            }
            case 8: {
                strcpy(system_info.terminal, "kitty");
                return system_info.terminal;
            }
            case 9: {
                strcpy(system_info.cpu, "i9-14900K");
                return system_info.cpu;
            }
            case 10: {
                strcpy(system_info.gpu, "rtx5090 32gb");
                return system_info.gpu;
            }
            case 11: {
                system_info.memory_max = 6767;
                system_info.memory_use = 4141;
                static char formatted[64];
                snprintf(formatted, sizeof(formatted), "Memory: %d/%d", system_info.memory_use, system_info.memory_max);
                return formatted;
            }
            case 12: {
                system_info.swap_max = 6767;
                system_info.swap_use = 4141;
                static char formatted[64];
                snprintf(formatted, sizeof(formatted), "Swap: %d/%d", system_info.swap_use, system_info.swap_max);
                return formatted;
            }
            case 13: {
                system_info.disk_max = 6767;
                system_info.disk_use = 4141;
                static char formatted[64];
                snprintf(formatted, sizeof(formatted), "Disk (/): %d/%d", system_info.disk_use, system_info.disk_max);
                return formatted;
            }
            case 14: {
                system_info.local_ip = 192.168;
                static char formatted[64];
                snprintf(formatted, sizeof(formatted), "Local IP: %f", system_info.local_ip);
                return formatted;
            }
            case 15: {
                system_info.battery = 67;
                static char formatted[64];
                snprintf(formatted, sizeof(formatted), "Battery: %d%%", system_info.battery);
                return formatted;
            }
            case 16: {
                strcpy(system_info.locale, "swedish");
                return system_info.locale;
            }
            case 99: {
                return "---------------";
            }
            default: {
                return "";
            }
        }
    } else {
        switch (modules_array[num]) {
            case 1:
                return system_info.hostname;
            case 2:
                return system_info.os;
            case 3:
                return system_info.kernel;
            case 4:
                static char formatted[64];
                snprintf(formatted, sizeof(formatted), "Uptime: %d", system_info.uptime);
                return formatted;
            case 5: {
                static char formatted[64];
                snprintf(formatted, sizeof(formatted), "Packages Installed: %d", system_info.package);
                return formatted;
            }
            case 6:
                return system_info.shell;
            case 7:
                return system_info.display;
            case 8:
                return system_info.terminal;
            case 9:
                return system_info.cpu;
            case 10:
                return system_info.gpu;
            case 11: {
                static char formatted[64];
                snprintf(formatted, sizeof(formatted), "Memory: %d/%d", system_info.memory_use, system_info.memory_max);
                return formatted;
            }
            case 12: {
                static char formatted[64];
                snprintf(formatted, sizeof(formatted), "Swap: %d/%d", system_info.swap_use, system_info.swap_max);
                return formatted;
            }
            case 13: {
                static char formatted[64];
                snprintf(formatted, sizeof(formatted), "Disk (/): %d/%d", system_info.disk_use, system_info.disk_max);
                return formatted;
            }
            case 14: {
                static char formatted[64];
                snprintf(formatted, sizeof(formatted), "Local IP: %f", system_info.local_ip);
                return formatted;
            }
            case 15: {
                static char formatted[64];
                snprintf(formatted, sizeof(formatted), "Battery: %d%%", system_info.battery);
                return formatted;
            }
            case 16:
                return system_info.locale;
            case 99:
                return "---------------";
            default:
                return "";
        }
    }
}

int modules = 24;

int main() {
    initscr();
    start_color();
    use_default_colors(); 

    init_pair(1, COLOR_WHITE, -1);
    init_pair(2, COLOR_MAGENTA, -1);
    init_pair(3, COLOR_BLACK, -1);

    parse_config("livefetch.conf");

    bool updating_visualizer = get_updating_visualizer();

    for (int num = 0; num < 32; num++) {
        modules_array[num] = get_module(num);
    }

    FILE* file = fopen(get_logo(), "r");

    clear();

    char line[256];
    char logo[modules][256];
    int lines = 0;
    if (file != NULL) {
        while (fgets(line, sizeof(line), file)) {
            if (lines == 0) {
                bool is_color = true;
                char identifier[7] = "COLOR:";
                int i = 0;
                for (; i < 6; i++) {
                    if (identifier[i] != line[i]) {
                        is_color = false;
                    }
                }

                i++;

                char color[16];
                if (is_color) {
                    int color_i = 0;
                    while (line[i]) {
                        if (line[i] != '\n') {
                            color[color_i] = line[i];
                        }
                        color_i++;
                        i++;
                    }

                    if (strcmp(color, "RED") == 0)
                        init_pair(2, COLOR_RED, -1);
                    else if (strcmp(color, "GREEN") == 0)
                        init_pair(2, COLOR_GREEN, -1);
                    else if (strcmp(color, "YELLOW") == 0)
                        init_pair(2, COLOR_YELLOW, -1);
                    else if (strcmp(color, "BLUE") == 0)
                        init_pair(2, COLOR_BLUE, -1);
                    else if (strcmp(color, "MAGENTA") == 0)
                        init_pair(2, COLOR_MAGENTA, -1);
                    else if (strcmp(color, "CYAN") == 0)
                        init_pair(2, COLOR_CYAN, -1);
                    else
                        init_pair(2, COLOR_WHITE, -1);
                }

                lines++;
            } else {
                strcpy(logo[lines -1 ], line);
                lines++;
            }
        }

        lines--;
    }

    int line_to_update = 0;
    while (1) {
        int i = 0;
        for (; i < modules;) {
            attron(COLOR_PAIR((i == line_to_update && updating_visualizer) ? 3 : 2));
            
            int chars_displayed = 0;
            if (i < lines) {
                int j = 0;
                while (logo[i][j]) {
                    if (logo[i][j] != '\n') {
                        printw("%c", logo[i][j]);
                    } else {
                        chars_displayed--;
                    }
                    chars_displayed++;
                    j++;
                }
            }

            for (; chars_displayed < 50; chars_displayed++) {
                printw(" ");
            }

            attron(COLOR_PAIR((i == line_to_update && updating_visualizer) ? 3 : 1));

            printw("%s", module(i, (i == line_to_update)));
            printw("\n");

            i++;
        }

        refresh();
        move(0, 0);

        if (line_to_update < modules) {
            line_to_update++;
        } else {
            line_to_update = 0;
        }

        napms(100);
    }
    
    for(;;);

    endwin();
}