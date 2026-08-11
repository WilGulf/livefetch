#include "ncurses.h"
#include "panel.h"

#include <string.h>
#include <unistd.h>
#include <stdarg.h>
#include <locale.h>
#include <stdlib.h>
#include <dirent.h>
#include <math.h>

#include "config.h"
#include "sysinfo.h"
#include "paths.h"
#include "globals.h"
#include "util.h"

#define MAJOR_VERSION 2
#define MINOR_VERSION 1
#define VERSION_PATCH 5

#define MAX_ROWS 64
#define MAX_COLS 256

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
    get_display(&system_info);

    get_uptime(&system_info);

    /*strcpy(system_info.shell, "bash");
    strcpy(system_info.display, "4K @ 240hz");
    strcpy(system_info.terminal, "kitty");*/
    get_mem(&system_info);
    get_swap(&system_info);
    get_disk(&system_info, "/");

    get_shell(&system_info);
    get_packages(&system_info);

    get_local_ip(&system_info);
    get_locale(&system_info);

    get_battery(&system_info);

    return 1;
}

void two_color_print(char *str, char *fmt, int color, ...) {
    va_list list;
    va_start(list, color);

    printw("%s", str);
    attron(COLOR_PAIR(color));
    char buffer[256];
    vsnprintf(buffer, sizeof(buffer) - 1, fmt, list);
    printw("%s", buffer);

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
                break;
            }
            case 3: {
                two_color_print("Kernel: ", "%s", (color == BLACK) ? BLACK : WHITE, system_info.kernel);
                break;
            }
            case 4: {
                get_uptime(&system_info);
                two_color_print("Uptime:", "%s", (color == BLACK) ? BLACK : WHITE, system_info.uptime);
                break;
            }
            case 5: {
                get_packages(&system_info);
                char buffer[32] = "";
                snprintf(buffer, sizeof(buffer), "Packages Installed (%s): ", system_info.package_man);
                two_color_print(buffer, "%s", (color == BLACK) ? BLACK : WHITE, system_info.package);
                break;
            }
            case 6: {
                two_color_print("System Shell: ", "%s", (color == BLACK) ? BLACK : WHITE, system_info.shell);
                break;
            }
            case 7: {
                get_display(&system_info);
                two_color_print("Display: ", "%s", (color == BLACK) ? BLACK : WHITE, system_info.display);
                break;
            }
            case 8: {
                //two_color_print("Terminal: ", "%s", (color == BLACK) ? BLACK : WHITE, system_info.terminal);
                break;
            }
            case 9: {
                two_color_print("CPU: ", "%s", (color == BLACK) ? BLACK : WHITE, system_info.cpu);
                break;
            }
            case 10: {
                two_color_print("GPU: ", "%s", (color == BLACK) ? BLACK : WHITE, system_info.gpu);
                break;
            }
            case 11: {
                get_mem(&system_info);
                two_color_print("Memory: ", "%s", (color == BLACK) ? BLACK : WHITE, system_info.memory);
                break;
            }
            case 12: {
                get_swap(&system_info);
                two_color_print("Swap: ", "%s", (color == BLACK) ? BLACK : WHITE, system_info.swap);
                break;
            }
            case 13: {
                get_disk(&system_info, "/");
                char buffer[32] = "";
                snprintf(buffer, sizeof(buffer), "Disk %s: ", system_info.disk);
                two_color_print(buffer, "%s", (color == BLACK) ? BLACK : WHITE, system_info.disk_info);
                break;
            }
            case 14: {
                get_local_ip(&system_info);
                two_color_print("Local IP: ", "%s", (color == BLACK) ? BLACK : WHITE, system_info.local_ip);
                break;
            }
            case 15: {
                get_battery(&system_info);

                if (system_info.battery == 111) {
                    two_color_print("Battery: ", "%s", (color == BLACK) ? BLACK : WHITE, "Running on AC");
                } else {
                    two_color_print("Battery: ", "%d", (color == BLACK) ? BLACK : system_info.battery_color, system_info.battery);
                    printw("%%");
                }
                
                break;
            }
            case 16: {
                two_color_print("System Locale: ", "%s", (color == BLACK) ? BLACK : WHITE, system_info.locale);
                break;
            }
            case 99: {
                attron(COLOR_PAIR(color));
                printw("---------------");
                break;
            }
            default: {
                //printw("");
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
            case 5: {
                char buffer[32] = "";
                snprintf(buffer, sizeof(buffer), "Packages Installed (%s): ", system_info.package_man);
                two_color_print(buffer, "%s", WHITE, system_info.package);
                break;
            }
            case 6:
                two_color_print("System Shell: ", "%s", WHITE, system_info.shell);
                break;
            case 7:
                two_color_print("Display: ", "%s", WHITE, system_info.display);
                break;
            case 8:
                //two_color_print("Terminal: ", "%s", WHITE, system_info.terminal);
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
            case 15: {
                if (system_info.battery == 111) {
                    two_color_print("Battery: ", "%s", WHITE, "Running on AC");
                } else {
                    two_color_print("Battery: ", "%d", system_info.battery_color, system_info.battery);
                    printw("%%");
                }
                
                break;
            }
            case 16:
                two_color_print("System Locale: ", "%s", WHITE, system_info.locale);
                break;
            case 99:
                attron(COLOR_PAIR(color));
                printw("---------------");
                break;
            default:
                //printw("");
                break;
        }
    }
}

#define Y 1
#define X 2

int spin_axis = Y;

typedef struct {
    char c;
    int color;
} Cell;

int center_x;
int center_y;

int longest_line = 0;
int lines = 0;

const char RAMP[] = ".:-=+*#%@";
const int RAMP_LEN = sizeof(RAMP) - 1;

void rotate_logo(Cell logo[MAX_ROWS][MAX_COLS], Cell dest[MAX_ROWS][MAX_COLS], float angle) {
    Cell temp[MAX_ROWS][MAX_COLS];
    float z_buffer[MAX_ROWS][MAX_COLS];

    memset(temp, 0, sizeof(temp));
    for (int i = 0; i < MAX_ROWS; i++) {
        for (int j = 0; j < MAX_COLS; j++) {
            z_buffer[i][j] = -9999.0f;
        }
    }

    for (int i = 0; i < MAX_ROWS; i++) {
        int j = 0;
        for (; j < longest_line; j++) {
            temp[i][j].c = ' ';
        }
        temp[i][j].c = '\0';
    }

    for (int y = 0; y < MAX_ROWS; y++) {
        for (int x = 0; x < MAX_COLS; x++) {
            if (logo[y][x].c != '\0' && logo[y][x].c != ' ') {
                float temp_x = x - center_x;
                float temp_y = y - center_y;
                
                float new_x = 0.0;
                float new_y = 0.0;
                float new_z = 0.0;

                if (spin_axis == X) {
                    new_x = temp_x;
                    new_y = temp_y * cos(angle);
                    new_z = temp_y * sin(angle);
                } else {
                    new_x = temp_x * cos(angle);
                    new_y = temp_y;
                    new_z = -temp_x * sin(angle);
                }
                
                int target_col = round(new_x) + center_x;
                int target_row = round(new_y) + center_y;

                if (0 <= target_col && target_col < MAX_COLS) {
                    if (z_buffer[target_row][target_col] <= new_z) {
                        z_buffer[target_row][target_col] = new_z;
                        temp[target_row][target_col] = logo[y][x];
                    }
                }
            }
        }
    }

    for (int i = 0; i < MAX_ROWS; i++) {
        for (int j = 0; j < MAX_COLS; j++) {
            dest[i][j] = temp[i][j];
        }
    }
}

void show_help() {
    printf("Livefetch is a fastfetch-like tool for fetching system information live in a pretty way\n");
    printf("\n");
    printf("\033[1;4mUsage:\033[0m\033[1m livefetch\033[0m <?options>\n");
    printf("\n");
    printf("\033[1;4mOptions:\033[0m\n");
    printf("\033[1m   -h, --help            \033[0m");
    printf("Show this help message\n");
    printf("\033[1m   -v, --version         \033[0m");
    printf("Print livefetch version\n");
    printf("\033[1m   -l, --logo <logo>     \033[0m");
    printf("Set the logo display. Use 'none' to disable logo output\n");
    printf("\033[1m   -c, --config <path>   \033[0m");
    printf("Specify the config file to load\n");
    printf("\033[1m   --list-modules        \033[0m");
    printf("List all available modules\n");
    printf("\033[1m   --list-logos          \033[0m");
    printf("List all printable logos\n");
    printf("\n");
    printf("\033[1m   --updating-visualizer <bool>    \033[0m");
    printf("Set if the updating visualizer should be shown\n");
    printf("\033[1m   --force-update <bool>           \033[0m");
    printf("Force updates on slow modules\n");
    printf("\033[1m   --spin-logo <bool>              \033[0m");
    printf("Spins the logo around\n");
    printf("\033[1m   --spin-axis <X, Y>              \033[0m");
    printf("Change the which axis the logo spins around\n");
}

void list_available_modules() {
    printf("Available modules:\n");
    printf("    spacer\n");
    printf("    hostname\n");
    printf("    os\n");
    printf("    kernel\n");
    printf("    uptime\n");
    printf("    packages\n");
    printf("    shell\n");
    printf("    cpu\n");
    printf("    gpu\n");
    printf("    display\n");
    printf("    memory\n");
    printf("    swap\n");
    printf("    disk\n");
    printf("    local ip\n");
    printf("    locale\n");
    printf("    battery\n");
}

void list_available_logos() {
    printf("Available logos:\n");
    DIR *dir = opendir(get_logo_path());
    if (dir) {
        struct dirent *ent;
        while (ent = readdir(dir)) {
            if (ent) {
                char *ext = strstr(ent->d_name, ".txt");
                if (ext) {
                    char buffer[64] = "";
                    int len = strlen(ent->d_name) - 4;
                    strncpy(buffer, ent->d_name, (len < 64) ? len : 64);
                    printf("    %s\n", buffer);
                }
            }
        }

        closedir(dir);
    }
}

int modules = 0;
int main_color = 7;

int main(int argc, char *argv[]) {
    for (int args_i = 0; args_i < argc; args_i++) {
        if ((strcmp(argv[args_i], "-h") == 0) || (strcmp(argv[args_i], "--help") == 0)) {
            show_help();
            return 0;
        } else if ((strcmp(argv[args_i], "-v") == 0) || (strcmp(argv[args_i], "--version") == 0)) {
            printf("Livefetch %d.%d.%d (%s)\n", MAJOR_VERSION, MINOR_VERSION, VERSION_PATCH, ARCH);
            return 0;
        } else if (strcmp(argv[args_i], "--list-modules") == 0) {
            list_available_modules();
            return 0;
        } else if (strcmp(argv[args_i], "--list-logos") == 0) {
            list_available_logos();
            return 0;
        }
    }

    // INIT //
    initscr();
    init_modules();
    nodelay(stdscr, TRUE); 
    start_color();
    curs_set(0);
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
            if (args_i + 1 < argc) {
                parse_config(argv[args_i + 1]);
                config_arg = true;
            }
        }
    }
    if (!config_arg) {
        parse_config(get_config_path());
    }

    bool updating_visualizer = true;
    bool spin_logo = false;

    // LOGO //
    FILE* file;
    bool logo_arg = false;
    bool force_update_arg = false;
    bool updating_visualizer_arg = false;
    bool spin_logo_arg = false;
    for (int args_i = 0; args_i < argc; args_i++) {
        if ((strcmp(argv[args_i], "-l") == 0) || (strcmp(argv[args_i], "--logo") == 0)) {
            if (args_i + 1 < argc) {
                file = fopen(get_logo(argv[args_i + 1], &system_info), "r");
                logo_arg = true;
            }
        } else if (strcmp(argv[args_i], "--force-update") == 0) {
            if (args_i + 1 < argc) {
                force_update_arg = true;
                if (string_is_statement(true, argv[args_i + 1]) || string_is_statement(false, argv[args_i + 1])) {
                    force_update = string_is_statement(true, argv[args_i + 1]);
                } else {
                    force_update_arg = false;
                }
            }
        } else if (strcmp(argv[args_i], "--updating-visualizer") == 0) {
            if (args_i + 1 < argc) {
                updating_visualizer_arg = true;
                if (string_is_statement(true, argv[args_i + 1]) || string_is_statement(false, argv[args_i + 1])) {
                    updating_visualizer = string_is_statement(true, argv[args_i + 1]);
                } else {
                    updating_visualizer_arg = false;
                }
            }
        } else if (strcmp(argv[args_i], "--spin-logo") == 0) {
            if (args_i + 1 < argc) {
                spin_logo_arg = true;
                if (string_is_statement(true, argv[args_i + 1]) || string_is_statement(false, argv[args_i + 1])) {
                    spin_logo = string_is_statement(true, argv[args_i + 1]);
                } else {
                    spin_logo_arg = false;
                }
            }
        } else if (strcmp(argv[args_i], "--spin-axis") == 0) {
            if (args_i + 1 < argc) {
                if (strcmp(argv[args_i + 1], "X") == 0 || strcmp(argv[args_i + 1], "x") == 0)
                    spin_axis = X;
                else if (strcmp(argv[args_i + 1], "Y") == 0 || strcmp(argv[args_i + 1], "y") == 0)
                    spin_axis = Y;
            }
        }
    }
    if (!logo_arg) {
        file = fopen(get_logo(NULL, &system_info), "r");
    }
    if (!force_update_arg) {
        force_update = get_value_bool("force_update", false);
    }
    if (!updating_visualizer_arg) {
        updating_visualizer = get_value_bool("updating_visualizer", true);
    }
    if (!spin_logo_arg) {
        spin_logo = get_value_bool("spin_logo", false);
    }

    // MODULES //
    for (int num = 0; num < 32; num++) {
        if (get_module(num) != 0) {
            modules_array[num] = get_module(num);
            modules++;
        }
    }

    clear();

    // LOAD LOGO //
    Cell logo[MAX_ROWS][MAX_COLS];
    memset(logo, 0, sizeof(logo));

    char line[256];
    
    if (file != NULL) {
        while (fgets(line, sizeof(line), file)) {
            bool is_color_line = false;
            int i = 0;
            int color_counter = 0;
            int current_color;

            if (strncmp(line, "MAIN_COLOR=", strlen("MAIN_COLOR=")) == 0) {
                is_color_line = true;
                i = strlen("MAIN_COLOR=");
            }

            if (is_color_line) {
                char color[32] = "";
                int j = 0;
                while (line[i] && line[i] != '\n') {
                    color[j] = line[i];
                    j++;
                    i++;
                }

                if (strcmp(color, "BLACK") == 0) main_color = BLACK;
                else if (strcmp(color, "RED") == 0) main_color = RED;
                else if (strcmp(color, "GREEN") == 0) main_color = GREEN;
                else if (strcmp(color, "YELLOW") == 0) main_color = YELLOW;
                else if (strcmp(color, "BLUE") == 0) main_color = BLUE;
                else if (strcmp(color, "MAGENTA") == 0) main_color = MAGENTA;
                else if (strcmp(color, "CYAN") == 0) main_color = CYAN;
                else if (strcmp(color, "WHITE") == 0) main_color = WHITE;

            } else {
                current_color = 0;
                color_counter = 0;
                int prev_longest = longest_line;
                while (line[i] && line[i] != '\n') {
                    if (line[i] == '$' && line[i + 1] != '\0') {
                        color_counter++;
                        current_color = line[i + 1] - '0';
                        i += 2;
                    } else {
                        logo[lines][i - (2 * color_counter)] = (Cell){line[i], current_color};
                        i++;
                    }
                }

                if (strlen(line) - (2 * color_counter) > (uint32_t)prev_longest) {
                    longest_line = strlen(line) - (2 * color_counter);            
                }
            }
            
            logo[lines][i - (2 * color_counter)] = (Cell){'\0', 0};
            is_color_line = false;
            lines++;
        }

        fclose(file);
    }

    center_x = longest_line / 2;
    center_y = lines / 2;

    Cell buffer[MAX_ROWS][MAX_COLS];
    for (int i = 0; i < lines; i++) {
        for (int j = 0; j < longest_line; j++) {
            buffer[i][j] = logo[i][j];
        }
    }

    // MAIN LOOP //
    int line_to_update = 0;
    float angle = 0;
    bool first = true;
    char c;
    while (1) {
        erase();
        attrset(A_NORMAL);

        if (spin_logo) {
            rotate_logo(logo, buffer, angle);
        }

        for (int i = 0; i < ((lines > modules) ? lines : modules);) {      
            for (int times = 0; times < 3; times++) {
                printw(" ");
            }

            int chars_displayed = 0;
            if (i < lines) {
                int j = 0;

                while (buffer[i][j].c) {
                    if (buffer[i][j].c != '\n') {
                        if (!(i == line_to_update && updating_visualizer)) {
                            attron(COLOR_PAIR((buffer[i][j].color)));
                        } else {
                            attron(COLOR_PAIR(BLACK));
                        }

                        printw("%c", buffer[i][j].c);
                        chars_displayed++;
                        j++;
                    }
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

        if (!first) {
            if (line_to_update < ((lines > modules) ? lines : modules)) {
                line_to_update++;
            } else {
                line_to_update = 0;
            }
        }
        first = !first;

        refresh();
        
        angle += 0.05;

        napms(50);
    }

    endwin();
}
