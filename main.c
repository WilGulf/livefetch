#include "ncurses.h"
#include "panel.h"

#include "string.h"
#include "unistd.h"
#include "stdarg.h"

#include "config.h"
#include "sysinfo.h"

struct sysinfo system_info;

int modules_array[32];

int init_modules() {
    get_hostname(&system_info);
    get_os(&system_info);
    get_kernel(&system_info);

    get_cpu(&system_info);
    get_gpu(&system_info);

    get_uptime(&system_info);

    strcpy(system_info.shell, "bash");
    strcpy(system_info.display, "4K @ 240hz");
    strcpy(system_info.terminal, "kitty");
    strcpy(system_info.gpu, "rtx5090 32gb");

    system_info.memory_max = 6767;
    system_info.memory_use = 4141;
    system_info.swap_max = 6767;
    system_info.swap_use = 4141;
    system_info.disk_max = 6767;
    system_info.disk_use = 4141;

    strcpy(system_info.local_ip, "192.168.1.67");

    system_info.battery = 67;

    strcpy(system_info.locale, "swedish");

    return 1;
}

void two_color_print(char *str, char *fmt, ...) {
    va_list list;
    va_start(list, fmt);

    printw(str);
    attron(COLOR_PAIR(1));
    char buffer[256];
    vsnprintf(buffer, sizeof(buffer) - 1, fmt, list);
    printw(buffer);

    va_end(list);
}

const char* module(int num, bool is_updating, int color) {
    if (is_updating) {
        switch (modules_array[num]) {
            attron(COLOR_PAIR(color));
            case 1: {
                printw("%s", system_info.hostname);
                break;
            }
            case 2: {
                printw("OS: %s", system_info.os);
                break;
            }
            case 3: {
                printw("Kernel: %s", system_info.kernel);
                break;
            }
            case 4: {
                get_uptime(&system_info);
                printw("Uptime: %s", system_info.uptime);
                break;
            }
            case 5: {
                system_info.package = 6767;
                printw("Packages Installed: %d", system_info.package);
                break;
            }
            case 6: {
                printw("Shell: %s", system_info.shell);
                break;
            }
            case 7: {
                printw("Display: %s", system_info.display);
                break;
            }
            case 8: {
                printw("Terminal: %s", system_info.terminal);
                break;
            }
            case 9: {
                printw("CPU: %s", system_info.cpu);
                break;
            }
            case 10: {
                printw("GPU: %s", system_info.gpu);
                break;
            }
            case 11: {
                system_info.memory_max = 6767;
                system_info.memory_use = 4141;
                printw("Memory: %d/%d", system_info.memory_use, system_info.memory_max);
                break;
            }
            case 12: {
                system_info.swap_max = 6767;
                system_info.swap_use = 4141;
                printw("Swap: %d/%d", system_info.swap_use, system_info.swap_max);
                break;
            }
            case 13: {
                system_info.disk_max = 6767;
                system_info.disk_use = 4141;
                printw("Disk (/): %d/%d", system_info.disk_use, system_info.disk_max);
                break;
            }
            case 14: {
                strcpy(system_info.local_ip, "192.168.1.67");
                printw("Local IP: %s", system_info.local_ip);
                break;
            }
            case 15: {
                system_info.battery = 67;
                printw("Battery: %d%%", system_info.battery);
                break;
            }
            case 16: {
                strcpy(system_info.locale, "swedish");
                printw("System Locale: %s", system_info.locale);
                break;
            }
            case 99: {
                attron(COLOR_PAIR(color));
                printw("---------------");
                break;
            }
            default: {
                printw("");
                break;;
            }
        }
    } else {
        switch (modules_array[num]) {
            case 1:
                attron(COLOR_PAIR(color));
                printw("%s", system_info.hostname);
                break;
            case 2:
                two_color_print("OS: ", "%s", system_info.os);
                break;
            case 3:
                two_color_print("Kernel: ", "%s", system_info.kernel);
                break;
            case 4:
                two_color_print("Uptime: ", "%s", system_info.uptime);
                break;
            case 5:
                two_color_print("Packages Installed: ", "%d", system_info.package);
                break;
            case 6:
                two_color_print("Shell: ", "%s", system_info.shell);
                break;
            case 7:
                two_color_print("Display: ", "%s", system_info.display);
                break;
            case 8:
                two_color_print("Terminal: ", "%s", system_info.terminal);
                break;
            case 9:
                two_color_print("CPU: ", "%s", system_info.cpu);
                break;
            case 10:
                two_color_print("GPU: ", "%s",  system_info.gpu);break;
            case 11:
                two_color_print("Memory: ", "%d/%d", system_info.memory_use, system_info.memory_max);
                break;
            case 12:
                two_color_print("Swap: ", "%d/%d", system_info.swap_use, system_info.swap_max);
                break;
            case 13:
                two_color_print("Disk (/): ", "%d/%d", system_info.disk_use, system_info.disk_max);
                break;
            case 14:
                two_color_print("Local IP: ", "%s", system_info.local_ip);
                break;
            case 15:
                two_color_print("Battery: ", "%d%%", system_info.battery);
                break;
            case 16:
                two_color_print("System Locale: ", "%s", system_info.locale);
                break;
            case 99:
                printw("---------------");
                break;
            default:
                printw("");
                break;
        }
    }
}

int modules = 0;

int main(int argc, char *argv[]) {
    // INIT //
    initscr();
    start_color();
    use_default_colors();

    init_pair(1, COLOR_WHITE, -1);
    init_pair(2, COLOR_MAGENTA, -1);
    init_pair(3, COLOR_BLACK, -1);

    // CONFIG //

    parse_config("livefetch.conf");
    bool updating_visualizer = get_updating_visualizer();

    // LOGO //
    FILE* file;
    bool logo_arg = false;
    for (int args_i = 0; args_i < argc; args_i++) {
        if (strcmp(argv[args_i], "-l") == 0) {
            file = fopen(get_logo(argv[args_i + 1]), "r");
            logo_arg = true;
        }
    }
    if (!logo_arg) {
        file = fopen(get_logo(NULL), "r");
    }

    // MODULES //
    for (int num = 0; num < 32; num++) {
        if (get_module(num) != 0) {
            modules_array[num] = get_module(num);
            modules++;
        }
    }

    init_modules();

    clear();


    char logo[64][256];
    char line[256];
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
                        } else {
                            color[color_i] = '\0';
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
                strcpy(logo[lines - 1], line);
                lines++;
            }
        }

        lines--;
    }

    int line_to_update = 0;
    while (1) {
        int i = 0;
        for (; i < ((lines > modules) ? lines : modules);) {
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

            module(i, (i == line_to_update), (i == line_to_update && updating_visualizer) ? 3 : 1);
            printw("\n");

            i++;
        }

        refresh();
        move(0, 0);

        if (line_to_update < ((lines > modules) ? lines : modules)) {
            line_to_update++;
        } else {
            line_to_update = 0;
        }

        napms(100);
    }
    
    for(;;);

    endwin();
}