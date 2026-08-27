#ifdef __linux__
#define _POSIX_C_SOURCE 200809L
#endif 

// SYSTEM
#ifdef _WIN32
#include <pdcurses.h>
#else
#include <ifaddrs.h>
#include <arpa/inet.h>
#endif
#include <string.h>
#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <assert.h>
#include <time.h>
#include <stdlib.h>
#include <ctype.h>

// LOCAL
#include "sysinfo.h"
#include "paths.h"
#include "util.h"
#include "globals.h"

#define RED 1
#define GREEN 2
#define YELLOW 3
#define BLUE 4
#define MAGENTA 5
#define CYAN 6
#define WHITE 7
#define BLACK 8

#ifdef _WIN32
#elif defined(__linux__)
    #include <sys/statvfs.h>
    #include <locale.h>
#elif defined(__APPLE__)
    #include <sys/sysctl.h>
    #include <mach/mach.h>
    #include <sys/mount.h>
    #include <CoreFoundation/CoreFoundation.h>
    #include <sys/socket.h>
    #include <IOKit/IOKitLib.h>
    #include <CoreGraphics/CGDirectDisplay.h>
    #include <CoreVideo/CVDisplayLink.h>

    #define ARRAY_SIZE(x) ({ static_assert(!__builtin_types_compatible_p(__typeof__(x), __typeof__(&*(x))), "Must not be a pointer"); (uint32_t) (sizeof(x) / sizeof(*(x))); })
#endif

void get_command_out(char *cmd, char *buffer) {
    FILE *fp;
    char path[1035];

    fp = popen(cmd, "r");
    if (fp == NULL) {
    } else {
        while (fgets(path, sizeof(path), fp) != NULL) {
            
        }
        
        int i = 0;
        while (path[i]) {
            if (path[i] == '\n') {
                buffer[i] = '\0';
                break;
            }

            buffer[i] = path[i];
            i++;
        }

        pclose(fp);
    }

}

int get_command_num_lines_out(char *cmd) {
    FILE *fp;
    char path[1035];

    int lines = 0;

    fp = popen(cmd, "r");
    if (fp != NULL) {
        while (fgets(path, sizeof(path), fp) != NULL) {
            lines++;
        }
    }

    pclose(fp);

    return lines;
}

void get_hostname(struct sysinfo *info) {
    char host[128] = "";
    char user[32] = "";
    get_command_out("uname -n", host);
    get_command_out("whoami", user);
    snprintf(info->hostname, sizeof(info->hostname), "%s@%s", user, host);
}

void get_os(struct sysinfo *info) {
#ifdef _WIN32

#elif defined(__linux__)
    FILE *file = fopen(path_exists("/etc/os-release") ? "/etc/os-release" : "/usr/lib/os-release", "r");
    if (file != NULL) {
        char line[256];
        char name[128] = "";
        char version[128] = "";
        while (fgets(line, sizeof(line), file)) {
            if (strncmp(line, "ID", 2) == 0 && line[2] == '=') {
                int i = 0;
                while (line[2 + i]) {
                    info->os_id[i] = line[2 + i];
                    i++;
                }

                info->os_id[i] = '\0';
            }

            if (strncmp(line, "NAME", 4) == 0 && line[4] == '=') {
                int i = 0;
                while (line[4 + i]) {
                    name[i] = line[4 + i];
                    i++;
                }
                name[i] = '\0';
            }
            if (strncmp(line, "VERSION", 7) == 0 && line[7] == '=') {
                int i = 0;
                while (line[7 + i]) {
                    version[i] = line[7 + i];
                    i++;
                }
                version[i] = '\0';
            }

            if (strncmp(line, "PRETTY_NAME", 11) == 0 && line[11] == '=') {
                int i = 0;
                while (line[11 + i]) {
                    info->os[i] = line[11 + i];
                    i++;
                }

                info->os[i] = '\0';
            }
            
        }

        if (strcmp(info->os, "") == 0) {
            snprintf(info->os, sizeof(info->os), "%s %s", name, version);
        }

        clean_string(info->os);
        clean_string(info->os_id);

        fclose(file);
    } else {
        //strncmp()
        strcpy(info->os_id, "linux_unknown");
    }
#elif defined(__APPLE__)
    char buffer[32];
    get_command_out("sw_vers -productVersion", buffer);
    snprintf(info->os, sizeof(info->os), "macOS %s", buffer);
#endif
}

void get_kernel(struct sysinfo *info) {
#ifdef _WIN32

#else
    get_command_out("uname -sr", info->kernel);
#endif
}

void get_uptime(struct sysinfo *info) {
double time = 0;
#ifdef _WIN32
#elif defined(__linux__)
    struct timespec ts;
    if (clock_gettime(CLOCK_BOOTTIME, &ts) == 0) {
        time = ts.tv_sec + ts.tv_nsec / 1e9;
    }
#elif defined(__APPLE__)
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    time = ts.tv_sec + ts.tv_nsec / 1e9;
#endif
    
    int seconds = time;
    int minutes = seconds / 60;
    int hours = minutes / 60;
    int days = hours / 24;

    char char_sec[32] = "";
    char char_min[32] = "";
    char char_h[32] = "";
    char char_day[32] = "";
    if (minutes > 0) {
        seconds -= (minutes * 60);
    }
    if (hours > 0) {
        minutes -= (hours * 60);
    }
    if (days > 0) {
        hours -= (days * 24);
    }

    snprintf(char_sec, sizeof(char_sec), " %d Seconds", seconds);
    snprintf(char_min, sizeof(char_min), " %d Minutes,", minutes);
    snprintf(char_h, sizeof(char_h), " %d Hours,", hours);
    snprintf(char_day, sizeof(char_day), " %d Days,", days);

    snprintf(info->uptime, sizeof(info->uptime), "%s%s%s%s", char_day, char_h, char_min, char_sec);
}

void get_cpu(struct sysinfo *info) {
    char cpu[64] = "";
    char cores[4] = "";
#ifdef _WIN32
#elif defined(__linux__)
    FILE *file = fopen("/proc/cpuinfo", "r");
    if (file != NULL) {
        char line[256] = "";
        while (fgets(line, sizeof(line), file)) {
            if (strncmp(line, "model name", 10) == 0) {
                int i = 10;
                while (line[i] != '\n' && line[i] != '\0') {
                    if (line[i] == ':') {
                        i++;
                        break;
                    }
                    i++;
                }

                if (line[i] != '\0') {
                    memmove(line, &line[i] + 1, strlen(&line[i]) + 1);
                    strcpy(cpu, line);
                }  
            } else if (strncmp(line, "cpu cores", 9) == 0) {
                int i = 10;
                while (line[i] != '\n' && line[i] != '\0') {
                    if (line[i] == ':') {
                        i++;
                        break;
                    }
                    i++;
                }

                if (line[i] != '\0') {
                    memmove(line, &line[i] + 1, strlen(&line[i] + 1) + 1);
                    strcpy(cores, line);
                }
            }
        }
        fclose(file);

        clean_string(cpu);
        clean_string(cores);
    }
#elif defined(__APPLE__)
    get_command_out("sysctl -n machdep.cpu.brand_string", cpu);
    get_command_out("sysctl -n hw.physicalcpu", cores);
#endif
    snprintf(info->cpu, sizeof(info->cpu), "%s (%s)", cpu, cores);
}

void get_gpu(struct sysinfo *info) {
    char gpu[64] = "";
#ifdef _WIN32
#elif defined(__linux__)
    char driver[32] = "";
    char pci_id[16] = "";
    char type[64] = "";
    
    FILE *file = fopen("/sys/class/drm/card0/device/uevent", "r");
    if (file) {
        char line[256] = "";
        while (fgets(line, sizeof(line), file)) {
            if (strncmp(line, "DRIVER=", 7) == 0) {
                strcpy(driver, line + 7);
                clean_string(driver);
            } else if (strncmp(line, "PCI_ID=", 7) == 0) {
                strcpy(pci_id, line + 7);
                clean_string(pci_id);
            }
        }

        if (pci_id[0]) {
            char cmd[128] = "";
            snprintf(cmd, sizeof(cmd), "lspci -d %s", pci_id);
            FILE *fp = popen(cmd, "r");
            if (fp) {
                if (fgets(line, sizeof(line), fp)) {
                    clean_string(line);

                    int brackets = 0;
                    for (char *tmp = line; *tmp; tmp++) {
                        if (*tmp == '[') {
                            brackets++;
                        }
                    }

                    if (brackets == 2) {
                        // AMD Discrete
                        // 0000:03:00.0 VGA compatible controller: Advanced Micro Devices, Inc. [AMD/ATI] Navi 23 [Radeon RX 6600] (rev c1)
                        char *tmp = strstr(line, "[AMD/ATI]") + strlen("[AMD/ATI]");
                        char *start = strchr(tmp, '[') + 1;
                        char *end = strchr(tmp, ']');
                        int len = end - start;
                        strncpy(gpu, start, len);
                    } else if (brackets == 1) {
                        if (strstr(line, "Corporation")) {
                            // NVIDIA
                            // 0000:01:00.0 VGA compatible controller: NVIDIA Corporation GA106 [GeForce RTX 3050 OEM] (rev a1)
                            char *start = strchr(line, '[') + 1;
                            char *end = strchr(line, ']');
                            int len = end - start;
                            strncpy(gpu, start, len);
                        } else {
                            // AMD Integrated
                            // 04:00.0 VGA compatible controller: Advanced Micro Devices, Inc. [AMD/ATI] Phoenix1 (rev c1)
                            char *start = strchr(line, ']') + 2;
                            char *end = strstr(line, "(rev") - 1;
                            int len = end - start;
                            strncpy(gpu, start, len);
                        }
                    } else {
                        // Intel
                        // 0000:00:02.0 VGA compatible controller: Intel Corporation Alder Lake-S GT1 (rev 0c)
                        char *start = (strstr(line, "Corporation ") + strlen("Corporation "));
                        char *end = strstr(line, "(rev") - 1;
                        int len = end - start;
                        strncpy(gpu, start, len);
                    }
                }

                pclose(fp);
            }
        }

        if (strcmp(driver, "i915") == 0 || strcmp(driver, "xe") == 0) {
            strcpy(type, "Integrated");
        } else if (strcmp(driver, "nvidia") == 0 || strcmp(driver, "nouveau") == 0) {
            strcpy(type, "Discrete");
        } else if (strcmp(driver, "amdgpu") == 0) {
            strcpy(type, "Discrete");

            if (
                strcmp(gpu, "Raphael") == 0 ||
                strcmp(gpu, "Renoir") == 0 ||
                strcmp(gpu, "Raven") == 0 ||
                strcmp(gpu, "Barcelo") == 0 ||
                strcmp(gpu, "Phoenix") == 0 ||
                strcmp(gpu, "Cezanne") == 0 ||
                strcmp(gpu, "Raven2") == 0 ||
                strcmp(gpu, "Strix Point") == 0 ||
                strcmp(gpu, "Phoenix2") == 0 ||
                strcmp(gpu, "Rembrandt") == 0 ||
                strcmp(gpu, "Van Gogh") == 0 ||
                strcmp(gpu, "Strix Halo") == 0 ||
                strcmp(gpu, "Hawk Point") == 0 ||
                strcmp(gpu, "Picasso") == 0 ||
                strcmp(gpu, "Mendocino") == 0 ||
                strcmp(gpu, "Krackan Point") == 0
            ) {
                strcpy(type, "Integrated");
            }
        }

        if (!gpu[0]) {
            strcpy(gpu, driver);
        }

        fclose(file);
    }

    snprintf(info->gpu, sizeof(info->gpu), "%s (%s)", gpu, type);
#elif defined(__APPLE__)
    int cores = 0;

    CFMutableDictionaryRef matchDict = IOServiceMatching("IOAccelerator");
    CFDictionaryAddValue(matchDict, CFSTR("IOMatchCategory"), CFSTR("IOAccelerator"));

    io_iterator_t iterator;

    if (IOServiceGetMatchingServices(kIOMainPortDefault, matchDict, &iterator) == kIOReturnSuccess)
    {
        io_registry_entry_t reg_entry;

        while ((reg_entry = IOIteratorNext(iterator))) {
            CFMutableDictionaryRef service_dictionary;
            if (IORegistryEntryCreateCFProperties(reg_entry, &service_dictionary, kCFAllocatorDefault, kNilOptions) != kIOReturnSuccess)
            {
                IOObjectRelease(reg_entry);
                continue;
            }
            CFTypeRef GPUModel = (CFTypeRef) CFDictionaryGetValue(service_dictionary, CFSTR("model"));
            CFTypeRef GPUCores = (CFTypeRef) CFDictionaryGetValue(service_dictionary, CFSTR("gpu-core-count"));

            if (GPUModel != nil) {
                if (CFGetTypeID(GPUModel) == CFStringGetTypeID()) {
                    CFStringRef cfStr = (CFStringRef) GPUModel;
                    
                    CFStringGetCString(cfStr, gpu, sizeof(gpu), kCFStringEncodingUTF8);
                }
            }

            if (GPUCores != nil) {
                if (CFGetTypeID(GPUCores) == CFNumberGetTypeID()) {
                    CFNumberGetValue((CFNumberRef) GPUCores, kCFNumberSInt32Type, &cores);
                }
            }
            CFRelease(service_dictionary);
            IOObjectRelease(reg_entry);
        }
        IOObjectRelease(iterator);
    }

    snprintf(info->gpu, sizeof(info->gpu), "%s (%d)", gpu, cores);
#endif
}

#define RED 1
#define GREEN 2
#define YELLOW 3
char *get_text_color(int color, char *text) {
    static char buffer[256];
    snprintf(buffer, sizeof(buffer), "\e[1;3%dm%s\e[0m", color, text);
    return buffer;
}

void bytes_to_barinfo(int64_t used_bytes, int64_t total_bytes, char *buffer, int size) {
    double mb_used = used_bytes / 1024 / 1024;
    double gb_used = mb_used / 1024;
    
    double mb_size = total_bytes / 1024 / 1024;
    double gb_size = mb_size / 1024;

    double temp = ((double)used_bytes / total_bytes);
    int percent = temp * 100;
    char bar_used[16] = "";
    char bar_avail[11] = "";
    char mem_used[16] = "";
    if (gb_used > 0) {
        snprintf(mem_used, sizeof(mem_used), "%.1f GB", gb_used);
    } else {
        snprintf(mem_used, sizeof(mem_used), "%.1f MB", mb_used);
    }

    char mem_size[16] = "";
    if (gb_size > 0) {
        snprintf(mem_size, sizeof(mem_size), "%.1f GB", gb_size);
    } else {
        snprintf(mem_size, sizeof(mem_size), "%.1f MB", mb_size);
    }

    int i = 0;
    for (; i < (percent / 10) && i <= 10; i++) {
        bar_used[i] = '#';
    }
    bar_used[i] = '\0';

    int j = 0;
    for (; i < 10; i++) {
        bar_avail[j] = '-';
        j++;
    }
    bar_avail[j] = '\0';

    snprintf(buffer, size, "(%s%s)   %s / %s", bar_used, bar_avail, mem_used, mem_size);
}

void get_mem(struct sysinfo *info) {
    int64_t total_bytes = 0;
    int64_t used_bytes = 0;
#ifdef _WIN32
#elif defined(__linux__)
    int64_t available_bytes = 0;
    FILE *file = fopen("/proc/meminfo", "r");
    if (file != NULL) {
        char line[256] = "";
        while (fgets(line, sizeof(line), file)) {
            if (strncmp(line, "MemTotal:", 9) == 0) {
                const char *start_ptr = strpbrk(line, "0123456789");
                if (start_ptr != NULL) {
                    char *end_ptr;
                    unsigned long long int result = strtoull(start_ptr, &end_ptr, 10);
                    total_bytes = (int64_t) result;
                }
            } else if (strncmp(line, "MemAvailable:", 8) == 0) {
                const char *start_ptr = strpbrk(line, "0123456789");
                if (start_ptr != NULL) {
                    char *end_ptr;
                    unsigned long long int result = strtoull(start_ptr, &end_ptr, 10);
                    available_bytes = (int64_t) result;
                }
            }        
        }

        total_bytes = total_bytes * 1024;
        available_bytes = available_bytes * 1024;
        used_bytes = total_bytes - available_bytes;

        fclose(file);        
    }
#elif defined(__APPLE__)
     size_t size;
    size = sizeof(int64_t);
    sysctlbyname("hw.memsize", &total_bytes, &size, NULL, 0);

    mach_port_t port = mach_host_self();
    vm_statistics64_data_t container;
    mach_msg_type_number_t count;
    count = HOST_VM_INFO64_COUNT;
    host_statistics64(port, HOST_VM_INFO64, (host_info64_t)&container, &count);

    vm_size_t page_size;
    host_page_size(port, &page_size);
    
    used_bytes = (
        (uint64_t)container.active_count +
        (uint64_t)container.wire_count +
        (uint64_t)container.compressor_page_count
    ) * page_size;
#endif

    bytes_to_barinfo(used_bytes, total_bytes, info->memory, sizeof(info->memory));
}

void get_swap(struct sysinfo *info) {
#ifdef _WIN32
#elif defined(__linux__)
    int64_t total_bytes = 0;
    int64_t used_bytes = 0;
    int64_t available_bytes = 0;

    FILE *file = fopen("/proc/meminfo", "r");
    if (file != NULL) {
        char line[256] = "";
        while (fgets(line, sizeof(line), file)) {
            if (strncmp(line, "SwapTotal:", 10) == 0) {
                const char *start_ptr = strpbrk(line, "0123456789");
                if (start_ptr != NULL) {
                    char *end_ptr;
                    unsigned long long int result = strtoull(start_ptr, &end_ptr, 10);
                    total_bytes = (int64_t) result;
                }
            } else if (strncmp(line, "SwapFree:", 9) == 0) {
                const char *start_ptr = strpbrk(line, "0123456789");
                if (start_ptr != NULL) {
                    char *end_ptr;
                    unsigned long long int result = strtoull(start_ptr, &end_ptr, 10);
                    available_bytes = (int64_t) result;
                }
            }        
        }

        total_bytes = total_bytes * 1024;
        available_bytes = available_bytes * 1024;
        used_bytes = total_bytes - available_bytes;

        fclose(file);        
    }

    bytes_to_barinfo(used_bytes, total_bytes, info->swap, sizeof(info->swap));
#elif defined(__APPLE__)
    struct xsw_usage swap;
    size_t size = sizeof(struct xsw_usage);
    sysctlbyname("vm.swapusage", &swap, &size, NULL, 0);
    bytes_to_barinfo(swap.xsu_used, swap.xsu_total, info->swap, sizeof(info->swap));
#endif
}

void get_disk(struct sysinfo *info, char *path) {
    int64_t used_bytes = 0;
    int64_t total_bytes = 0;
#ifdef _WIN32
#elif defined(__linux__)
    struct statvfs fs;
    if (statvfs(path, &fs) != 0) {
        memset(&fs, 0, sizeof(fs));
    }
    total_bytes = fs.f_blocks * (int64_t) fs.f_frsize;
    int64_t free_bytes = fs.f_bfree * (int64_t) fs.f_frsize;
    used_bytes = total_bytes - free_bytes;
#elif defined(__APPLE__)
    struct statfs disk_buffer;
    statfs(path, &disk_buffer);
    used_bytes = (disk_buffer.f_blocks - disk_buffer.f_bfree) * disk_buffer.f_bsize;
    total_bytes = disk_buffer.f_blocks * disk_buffer.f_bsize;
#endif
    char buffer[256] = "";
    bytes_to_barinfo(used_bytes, total_bytes, buffer, sizeof(buffer));
    snprintf(info->disk_info, sizeof(info->disk_info), "%s", buffer);
    snprintf(info->disk, sizeof(info->disk), "(%s)", path);
}

void get_locale(struct sysinfo *info) {
#ifdef __linux__
    setlocale(LC_ALL, "");
    snprintf(info->locale, sizeof(info->locale), "%s\n", setlocale(LC_ALL, NULL));
    clean_string(info->locale);
#elif defined(__APPLE__)
    CFLocaleRef buffer = CFLocaleCopyCurrent();
    CFStringRef value = CFLocaleGetValue(buffer, kCFLocaleIdentifier);
    char locale[64] = "";
    CFStringGetCString(value, locale, sizeof(locale), kCFStringEncodingUTF8);
    char *ptr = strchr(locale, '@');
    if (ptr != NULL) {
        *ptr = '\0';
    }
    snprintf(info->locale, sizeof(info->locale), "%s", locale);
    CFRelease(buffer);
#endif
}

void get_local_ip(struct sysinfo *info) {
#ifndef WIN32

#else
    bool ip_found = false;
    struct ifaddrs *interfaces;
    if (getifaddrs(&interfaces) == 0) {
        for (struct ifaddrs *ifa = interfaces; ifa != NULL; ifa = ifa->ifa_next) {
            if (ifa->ifa_addr == NULL) {
                continue;
            }
            if (ifa->ifa_addr->sa_family == AF_INET) {
                if (strcmp(ifa->ifa_name, "lo0") != 0) {
                    struct sockaddr_in *addr = (struct sockaddr_in *)ifa->ifa_addr;
                    char buffer[64] = "";
                    inet_ntop(AF_INET, &addr->sin_addr, buffer, sizeof(buffer));
                    snprintf(info->local_ip, sizeof(info->local_ip), "%s (%s)", buffer, ifa->ifa_name);
                    ip_found = true;
                }
            }
        }
    }
    freeifaddrs(interfaces);

    if (!ip_found) {
        snprintf(info->local_ip, sizeof(info->local_ip), "Unknown");
    }
#endif
}

void get_display(struct sysinfo *info) {
    char display[64] = "";
    char resolution[32] = "";
    int refresh_rate = 0;
#ifdef _WIN32
#elif defined(__linux__)
    DIR *dir = opendir("/sys/class/drm");
    if (dir) {
        struct dirent *ent;
        while (ent = readdir(dir)) {
            char path[256] = "";
            snprintf(path, sizeof(path), "/sys/class/drm/%s/status", ent->d_name);
            FILE *file = fopen(path, "r");
            
            if (file) {
                char line[256] = "";
                if (fgets(line, sizeof(line), file)) {
                    if (strncmp(line, "connected", 9) == 0) {
                        fclose(file);

                        char *name = ent->d_name + 6;
                        strcpy(display, name);

                        memset(path, 0, sizeof(path));
                        snprintf(path, sizeof(path), "/sys/class/drm/%s/modes", ent->d_name);
                        FILE *modes = fopen(path, "r");
                        if (modes) {
                            if (fgets(line, sizeof(line), modes)) {
                                stpcpy(resolution, line);
                            }

                            fclose(modes);
                        }
                        
                        break;
                    }
                }
            }
        }

        closedir(dir);
    }

    clean_string(resolution);
    snprintf(info->display, sizeof(info->display), "%s %s", display, resolution);
#elif defined(__APPLE__)
    
    CGDirectDisplayID screens[128];
    uint32_t screenCount;
    boolean_t is_builtin = false;
    size_t width = 0;
    size_t height = 0;

    if (CGGetOnlineDisplayList(ARRAY_SIZE(screens), screens, &screenCount) != 0) {
        return;
    }

    for (uint32_t i = 0; i < screenCount; i++) {
        CGDirectDisplayID screen = screens[i];
        is_builtin = CGDisplayIsBuiltin(screen);
        CGDisplayModeRef mode = CGDisplayCopyDisplayMode(screen);
        if (mode) {
            double mode_refresh_rate = CGDisplayModeGetRefreshRate(mode);
            width = CGDisplayModeGetPixelWidth(mode);
            height = CGDisplayModeGetPixelHeight(mode);

            refresh_rate = mode_refresh_rate;
        }
    }

    snprintf(display, sizeof(display), "%s", (is_builtin ? "BuiltIn" : "External"));
    snprintf(resolution, sizeof(resolution), "%zux%zu", width, height);

    snprintf(info->display, sizeof(info->display), "%s %s %dhz", display, resolution, refresh_rate);
#endif
}

void get_shell(struct sysinfo *info) {
#ifdef _WIN32
#else
    get_command_out("echo $SHELL", info->shell);
#endif
}

#if defined(__APPLE__) || defined(__linux__)
int32_t get_dirs_in_dir(DIR *dir) {
    int32_t num_elements = 0;

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        bool ok = false;

        if (entry->d_name[0] != '.') {
            {
                struct stat stbuf;
                if (fstatat(dirfd(dir), entry->d_name, &stbuf, 0) == 0) {
                    ok = S_ISDIR(stbuf.st_mode);
                }
            }
        }

        if (ok) {
            num_elements++;
        }
    }

    return num_elements;
}

int32_t get_files_in_dir(DIR *dir) {
    int32_t num_elements = 0;

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        bool ok = false;

        if (entry->d_name[0] != '.') {
            struct stat stbuf;
            if (fstatat(dirfd(dir), entry->d_name, &stbuf, 0) == 0) {
                ok = S_ISREG(stbuf.st_mode);
            }
        }

        if (ok) {
            num_elements++;
        }
    }

    return num_elements;
}
#endif

void get_packages(struct sysinfo *info) {
#ifdef _WIN32

#elif defined(__linux__)
    int64_t packages = 0;
    if (path_exists("/var/lib/rpm")) {
        if (strlen(info->package) == 0 || force_update) {
            packages = get_command_num_lines_out("rpm -qa");
            snprintf(info->package, sizeof(info->package), "%ld", packages);
        }
        strcpy(info->package_man, "rpm");

    } else if (path_exists("/var/lib/dpkg/status")) {
        FILE *file = fopen("/var/lib/dpkg/status", "r");
        if (file != NULL) {
            char line[256];
            while (fgets(line, sizeof(line), file)) {
                if (strncmp(line, "Status: install ok installed", 28) == 0) {
                    packages++;
                }
            }
            fclose(file);
        }
        strcpy(info->package_man, "apt");
        snprintf(info->package, sizeof(info->package), "%ld", packages);

    } else if (path_exists("/var/lib/pacman/local")) {
        DIR *dir = opendir("/var/lib/pacman/local");
        if (dir != NULL) {
            packages = get_dirs_in_dir(dir);
        }
        strcpy(info->package_man, "pacman");
        snprintf(info->package, sizeof(info->package), "%ld", packages);

    } else if (path_exists("/lib/apk/db/installed")) {
        FILE *file = fopen("/lib/apk/db/installed", "r");
        if (file != NULL) {
            char line[256];
            while (fgets(line, sizeof(line), file)) {
                if (strncmp(line, "P:", 2) == 0) {
                    packages++;
                }
            }

            fclose(file);
        }

        strcpy(info->package_man, "apk");
        snprintf(info->package, sizeof(info->package), "%ld", packages);

    } else if (path_exists("/var/db/xbps")) {
        if (strlen(info->package) == 0 || force_update) {
            packages = get_command_num_lines_out("xbps-query -l");
            snprintf(info->package, sizeof(info->package), "%ld", packages);
        }
        /*DIR *dir = opendir("/var/db/xbps");
        if (dir != NULL) {
            packages = get_files_in_dir(dir);
            closedir(dir);
        }*/
        strcpy(info->package_man, "xbps");
        //snprintf(info->package, sizeof(info->package), "%ld", packages);

    } else {
        strcpy(info->package_man, "Unknown");
    }
#endif
#ifdef __APPLE__ || __linux__
    const char* brew_prefix = getenv("HOMEBREW_PREFIX");

    if (brew_prefix != NULL) {
        strcpy(info->package_man, "brew");

        int32_t casks = 0;
        int32_t formulae = 0;
        char path[sizeof(brew_prefix) + 32] = "";
        snprintf(path, sizeof(path), "%s/Caskroom", brew_prefix);
        DIR *dir = opendir(path);
        if (dir != NULL) {
            casks += get_dirs_in_dir(dir);
            closedir(dir);
        }
        

        snprintf(path, sizeof(path), "%s/Cellar", brew_prefix);
        dir = opendir(path);
        if (dir != NULL) {
            formulae += get_dirs_in_dir(dir);
            closedir(dir);
        }  

        snprintf(info->package, sizeof(info->package), "%d formulae, %d casks", formulae, casks);
    }
#endif
}

void get_battery(struct sysinfo *info) {
#ifdef __linux__
    if (!path_exists("/sys/class/power_supply/BAT0")) {
        info->battery = 111;
    } else {
        FILE *file = fopen("/sys/class/power_supply/BAT0/capacity", "r");
        if (file != NULL) {
            char line[32];
            while (fgets(line, sizeof(line), file)) {
                const char *start_ptr = strpbrk(line, "0123456789");
                char *end_ptr;
                unsigned long long int result = strtoull(start_ptr, &end_ptr, 10);
                
                info->battery = (int) result;

                fclose(file);
            }
        }
    }
#elif defined(__APPLE__)
    CFMutableDictionaryRef matchDict = IOServiceMatching("AppleSmartBattery");
    CFDictionaryAddValue(matchDict, CFSTR("IOMatchCategory"), CFSTR("AppleSmartBattery"));

    io_iterator_t iterator;

    if (IOServiceGetMatchingServices(kIOMainPortDefault, matchDict, &iterator) == kIOReturnSuccess)
    {
        io_registry_entry_t reg_entry;

        while ((reg_entry = IOIteratorNext(iterator))) {
            CFMutableDictionaryRef service_dictionary;
            if (IORegistryEntryCreateCFProperties(reg_entry, &service_dictionary, kCFAllocatorDefault, kNilOptions) != kIOReturnSuccess)
            {
                IOObjectRelease(reg_entry);
                continue;
            }

            int currentCapacity = 0, maxCapacity = 0;

            CFTypeRef max_capacity_cf = (CFTypeRef) CFDictionaryGetValue(service_dictionary, CFSTR("MaxCapacity"));
            CFTypeRef current_capacity_cf = (CFTypeRef) CFDictionaryGetValue(service_dictionary, CFSTR("CurrentCapacity"));

            if (max_capacity_cf != nil) {
                if (CFGetTypeID(max_capacity_cf) == CFNumberGetTypeID()) {
                    CFNumberGetValue((CFNumberRef) max_capacity_cf, kCFNumberSInt32Type, &maxCapacity);
                }
            }

            if (current_capacity_cf != nil) {
                if (CFGetTypeID(current_capacity_cf) == CFNumberGetTypeID()) {
                    CFNumberGetValue((CFNumberRef) current_capacity_cf, kCFNumberSInt32Type, &currentCapacity);
                }
            }

            info->battery = currentCapacity * 100.0 / maxCapacity;

            CFRelease(service_dictionary);
            IOObjectRelease(reg_entry);
        }
        IOObjectRelease(iterator);
    }
#endif

    info->battery_color = (info->battery >= 40) ? GREEN : (info->battery >= 20) ? YELLOW : RED;
}