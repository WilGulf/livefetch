#include "ncurses.h"
#include "panel.h"

#include "string.h"
#include "unistd.h"
#include "stdarg.h"

#include "config.h"
#include "sysinfo.h"
#include "paths.h"

#define MAJOR_VERSION 0
#define MINOR_VERSION 1
#define VERSION_PATCH 0

#define RED 1
#define GREEN 2
#define YELLOW 3
#define BLUE 4
#define MAGENTA 5
#define CYAN 6
#define WHITE 7
#define BLACK 8

#define UPDATE 8
#define TEXT 7

#ifdef __i386__
    #define ARCH "i386"
#elif defined(__x86_64__)
    #define ARCH "x86_64"
#elif defined(__arm__)
    #define ARCH "arm"
#elif defined(__aarch64__)
    #define ARCH "aarch64"
#elif defined(__riscv)
    #define ARCH "RISC-V"
#elif defined(__powerpc__)
    #define ARCH "PowerPC"
#endif

struct sysinfo system_info;

int modules_array[32];

int init_modules() {
    get_hostname(&system_info);
    get_os(&system_info);
    get_kernel(&system_info);

    get_cpu(&system_info);
    get_gpu(&system_info);

    get_uptime(&system_info);

    /*strcpy(system_info.shell, "bash");
    strcpy(system_info.display, "4K @ 240hz");
    strcpy(system_info.terminal, "kitty");*/
    get_mem(&system_info);
    get_swap(&system_info);
    get_disk(&system_info, "/");

    get_local_ip(&system_info);
    get_locale(&system_info);

    system_info.battery = 67;

    return 1;
}

void two_color_print(char *str, char *fmt, int color, ...) {
    va_list list;
    va_start(list, color);

    printw(str);
    attron(COLOR_PAIR(color));
    char buffer[256];
    vsnprintf(buffer, sizeof(buffer) - 1, fmt, list);
    printw(buffer);

    va_end(list);
}

void module(int num, bool is_updating, int color) {
    if (is_updating) {
        switch (modules_array[num]) {
            attron(COLOR_PAIR(color));
            case 1: {
                attron(COLOR_PAIR((color == BLACK) ? BLACK : WHITE));
                printw("%s", system_info.hostname);
                break;
            }
            case 2: {
                two_color_print("OS: ", "%s", (color == BLACK) ? BLACK : WHITE, system_info.os);
                //printw("OS: %s", system_info.os);
                break;
            }
            case 3: {
                two_color_print("Kernel: ", "%s", (color == BLACK) ? BLACK : WHITE, system_info.kernel);
                //printw("Kernel: %s", system_info.kernel);
                break;
            }
            case 4: {
                get_uptime(&system_info);
                two_color_print("Uptime:", "%s", (color == BLACK) ? BLACK : WHITE, system_info.uptime);
                //printw("Uptime:%s", system_info.uptime);
                break;
            }
            case 5: {
                system_info.package = 6767;
                two_color_print("Packages Installed: ", "%d", (color == BLACK) ? BLACK : WHITE, system_info.package);
                //printw("Packages Installed: %d", system_info.package);
                break;
            }
            case 6: {
                two_color_print("Shell: ", "%s", (color == BLACK) ? BLACK : WHITE, system_info.shell);
                //printw("Shell: %s", system_info.shell);
                break;
            }
            case 7: {
                two_color_print("Display: ", "%s", (color == BLACK) ? BLACK : WHITE, system_info.display);
                //printw("Display: %s", system_info.display);
                break;
            }
            case 8: {
                two_color_print("Terminal: ", "%s", (color == BLACK) ? BLACK : WHITE, system_info.terminal);
                //printw("Terminal: %s", system_info.terminal);
                break;
            }
            case 9: {
                two_color_print("CPU: ", "%s", (color == BLACK) ? BLACK : WHITE, system_info.cpu);
                //printw("CPU: %s", system_info.cpu);
                break;
            }
            case 10: {
                two_color_print("CPU: ", "%s", (color == BLACK) ? BLACK : WHITE, system_info.cpu);
                //printw("GPU: %s", system_info.gpu);
                break;
            }
            case 11: {
                get_mem(&system_info);
                two_color_print("Memory: ", "%s", (color == BLACK) ? BLACK : WHITE, system_info.memory);
                //printw("Memory: %s", system_info.memory);
                break;
            }
            case 12: {
                get_swap(&system_info);
                two_color_print("Swap: ", "%s", (color == BLACK) ? BLACK : WHITE, system_info.swap);
                //printw("Swap: %s", system_info.swap);
                break;
            }
            case 13: {
                get_disk(&system_info, "/");
                char buffer[32] = "";
                snprintf(buffer, sizeof(buffer), "Disk %s: ", system_info.disk);
                two_color_print(buffer, "%s", (color == BLACK) ? BLACK : WHITE, system_info.disk_info);
                //printw("Disk %s: %s", system_info.disk, system_info.disk_info);
                break;
            }
            case 14: {
                get_local_ip(&system_info);
                two_color_print("Local IP: ", "%s", (color == BLACK) ? BLACK : WHITE, system_info.local_ip);
                //printw("Local IP: %s", system_info.local_ip);
                break;
            }
            case 15: {
                system_info.battery = 67;
                two_color_print("Battery: ", "%d%%", (color == BLACK) ? BLACK : WHITE, system_info.battery);
                //printw("Battery: %d%%", system_info.battery);
                break;
            }
            case 16: {
                two_color_print("System Locale: ", "%s", (color == BLACK) ? BLACK : WHITE, system_info.locale);
                //printw("System Locale: %s", system_info.locale);
                break;
            }
            case 99: {
                attron((color == BLACK) ? BLACK : WHITE);
                printw("---------------");
                break;
            }
            default: {
                printw("");
                break;;
            }
        }
    } else {
        attron(COLOR_PAIR(color));
        switch (modules_array[num]) {
            case 1:
                attron(COLOR_PAIR(WHITE));
                printw("%s", system_info.hostname);
                break;
            case 2:
                two_color_print("OS: ", "%s", WHITE, system_info.os);
                break;
            case 3:
                two_color_print("Kernel: ", "%s", WHITE, system_info.kernel);
                break;
            case 4:
                two_color_print("Uptime:", "%s", WHITE, system_info.uptime);
                break;
            case 5:
                two_color_print("Packages Installed: ", "%d", WHITE, system_info.package);
                break;
            case 6:
                two_color_print("Shell: ", "%s", WHITE, system_info.shell);
                break;
            case 7:
                two_color_print("Display: ", "%s", WHITE, system_info.display);
                break;
            case 8:
                two_color_print("Terminal: ", "%s", WHITE, system_info.terminal);
                break;
            case 9:
                two_color_print("CPU: ", "%s", WHITE, system_info.cpu);
                break;
            case 10:
                two_color_print("GPU: ", "%s", WHITE,  system_info.gpu);break;
            case 11:
                two_color_print("Memory: ", "%s", WHITE, system_info.memory);
                break;
            case 12:
                two_color_print("Swap: ", "%s", WHITE, system_info.swap);
                break;
            case 13: {
                char buffer[32] = "";
                snprintf(buffer, sizeof(buffer), "Disk %s: ", system_info.disk);
                two_color_print(buffer, "%s", WHITE, system_info.disk_info);
                break;
            }
            case 14:
                two_color_print("Local IP: ", "%s", WHITE, system_info.local_ip);
                break;
            case 15:
                two_color_print("Battery: ", "%d%%", WHITE, system_info.battery);
                break;
            case 16:
                two_color_print("System Locale: ", "%s", WHITE, system_info.locale);
                break;
            case 99:
                attron(COLOR_PAIR(color));
                printw("---------------");
                break;
            default:
                printw("");
                break;
        }
    }
}

int modules = 0;
int main_color = 7;

int main(int argc, char *argv[]) {
    for (int args_i = 0; args_i < argc; args_i++) {
        if ((strcmp(argv[args_i], "-h") == 0) || (strcmp(argv[args_i], "--help") == 0)) {
            printf("Livefetch is a fastfetch-like tool for fetching system information live in a pretty way\n");
            printf("\n");
            printf("\033[1;4mUsage:\033[0m\033[1m livefetch\033[0m <?options>\n");
            printf("\n");
            printf("\033[1;4mOptions:\033[0m\n");
            printf("\033[1m   -h, --help     \033[0m");
            printf("Show this help message\n");
            printf("\033[1m   -v, --version  \033[0m");
            printf("Show the full Livefetch version\n");
            printf("\033[1m   -l, --logo     \033[0m");
            printf("Set the logo source. Use 'none' to disable logo output\n");
            printf("\033[1m   -c, --config   \033[0m");
            printf("Specify the config file to load\n");
            return 1;
        } else if ((strcmp(argv[args_i], "-v") == 0) || (strcmp(argv[args_i], "--version") == 0)) {
            printf("Livefetch %d.%d.%d (%s)\n", MAJOR_VERSION, MINOR_VERSION, VERSION_PATCH, ARCH);
            return 1;
        }
    }

    // INIT //
    initscr();
    start_color();
    use_default_colors();
    init_pair(RED, COLOR_RED, -1);
    init_pair(GREEN, COLOR_GREEN, -1);
    init_pair(YELLOW, COLOR_YELLOW, -1);
    init_pair(BLUE, COLOR_BLUE, -1);
    init_pair(MAGENTA, COLOR_MAGENTA, -1);
    init_pair(CYAN, COLOR_CYAN, -1);
    init_pair(WHITE, COLOR_WHITE, -1);
    init_pair(BLACK, COLOR_BLACK, -1);

    // CONFIG //
    bool config_arg = false;
    for (int args_i = 0; args_i < argc; args_i++) {
        if ((strcmp(argv[args_i], "-c") == 0) || (strcmp(argv[args_i], "--config") == 0)) {
            parse_config(argv[args_i + 1]);
            config_arg = true;
        }
    }
    if (!config_arg) {
        parse_config(CONFIG_PATH);
    }

    bool updating_visualizer = get_updating_visualizer();

    // LOGO //
    FILE* file;
    bool logo_arg = false;
    for (int args_i = 0; args_i < argc; args_i++) {
        if ((strcmp(argv[args_i], "-l") == 0) || (strcmp(argv[args_i], "--logo") == 0)) {
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

    // LOAD LOGO //
    char logo[64][256];
    char line[256];
    int longest_line = 0;
    int lines = 0;
    if (file != NULL) {
        while (fgets(line, sizeof(line), file)) {
            bool is_color_line = false;
            int i = 0;
            char identifier[] = "MAIN_COLOR=";
            for (; (uint64_t)i < (strlen(identifier)); i++) {
                if (line[i] == identifier[i]) {
                    is_color_line = true;
                } else {
                    is_color_line = false;
                    break;
                }
            }

            if (is_color_line) {
                char color[32] = "";
                int j = 0;
                while (line[i] && line[i] != '\n') {
                    color[j] = line[i];
                    j++;
                    i++;
                }

                if (strcmp(color, "BLACK") == 0)
                    main_color = BLACK;
                else if (strcmp(color, "RED") == 0)
                    main_color = RED;
                else if (strcmp(color, "GREEN") == 0)
                    main_color = GREEN;
                else if (strcmp(color, "YELLOW") == 0)
                    main_color = YELLOW;
                else if (strcmp(color, "BLUE") == 0)
                    main_color = BLUE;
                else if (strcmp(color, "MAGENTA") == 0)
                    main_color = MAGENTA;
                else if (strcmp(color, "CYAN") == 0)
                    main_color = CYAN;
                else if (strcmp(color, "WHITE") == 0)
                    main_color = WHITE;

            } else {
                int j = 0;
                int color_counter = 0;
                int prev_longest = longest_line;
                while (line[j]) {
                    if (line[j] == '$') {
                        color_counter++;
                    }
                    //logo[lines][j] = line[j];
                    j++;
                }
                strcpy(logo[lines], line);

                if (strlen(line) - (2 * color_counter) > (uint32_t)prev_longest) {
                    longest_line = strlen(line) - (2 * color_counter);            
                }

                lines++;
            }
            is_color_line = false;
        }
    }

    int line_to_update = 0;
    while (1) {
        erase();

        for (int i = 0; i < ((lines > modules) ? lines : modules);) {      
            for (int times = 0; times < 3; times++) {
                printw(" ");
            }

            int chars_displayed = 0;
            if (i < lines) {
                int j = 0;
                while (logo[i][j]) {
                    if (logo[i][j] != '\n') {
                        if (logo[i][j] == '$') {
                            if (!(i == line_to_update && updating_visualizer)) {
                                attron(COLOR_PAIR((logo[i][j + 1] - '0')));
                            } else {
                                attron(COLOR_PAIR(BLACK));
                            }

                            //
                            j++;
                        } else {
                            printw("%c", logo[i][j]);
                            chars_displayed++;
                        }
                    }
                    j++;
                }
            } else {
                if (!(i == line_to_update && updating_visualizer)) {
                    attron(COLOR_PAIR(main_color));
                } else {
                    attron(COLOR_PAIR(BLACK));
                }
            }

            for (; chars_displayed < ((longest_line > 0) ? longest_line + 5 : 0); chars_displayed++) {
                printw(" ");
            }

            attron(COLOR_PAIR((i == line_to_update && updating_visualizer) ? BLACK : main_color));
            module(i, (i == line_to_update), (i == line_to_update && updating_visualizer) ? BLACK : main_color);
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