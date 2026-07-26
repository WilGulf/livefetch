#include "paths.h"

#include "string.h"
#include "unistd.h"
#include "stdarg.h"
#include "sys/stat.h"
#include "stdbool.h"
#include "stdio.h"

#ifdef __APPLE__
#include "mach-o/dyld.h"
#include "limits.h"
#include "libgen.h"
#endif

bool path_exists(const char *filename) {
    struct stat buffer;
    return stat(filename, &buffer) == 0 ? true : false;
}

static void get_binary_dir(char *buffer, size_t size) {
#ifdef __linux__
    ssize_t len = readlink("/proc/self/exe", buffer, size - 1);

    if (len != -1) {
        buffer[len] = '\0';
    }
#elif defined(__APPLE__)
    uint32_t len = size;

    _NSGetExecutablePath(buffer, &len);
#endif
    char *slash = strrchr(buffer, '/');

    if (slash)
        *slash = '\0';
}

const char *get_config_path(void) {
    static char path[PATH_MAX];

    if (path_exists(DATA_DIR"/default.conf")) {
        return DATA_DIR "/default.conf";
    } else {
        char bin_dir[PATH_MAX];
        get_binary_dir(bin_dir, sizeof(bin_dir));

        snprintf(path, sizeof(path), "%s/../share/livefetch/default.conf", bin_dir);

        return path;
    }
}

const char *get_logo_path(void) {
    static char path[PATH_MAX];
    
    if (path_exists(DATA_DIR"/logos")) {
        return DATA_DIR "/logos";
    } else {
        char bin_dir[PATH_MAX];
        get_binary_dir(bin_dir, sizeof(bin_dir));

        snprintf(path, sizeof(path), "%s/../share/livefetch/logos", bin_dir);

        return path;
    }
}