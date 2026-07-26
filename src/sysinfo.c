#ifdef __linux__
#define _POSIX_C_SOURCE 200809L
#endif 

#include "sysinfo.h"

#include <string.h>
#include <stdio.h>
#include <ncurses.h>
#include <dirent.h>
#include <sys/stat.h>
#include <assert.h>

#ifdef __linux__
    // I USE LINUX BTW, FUCK WINDOWS
    #include <stdlib.h>
#elif defined(__APPLE__)
    #include <stdlib.h>
    #include <sys/sysctl.h>
    #include <time.h>
    #include <mach/mach.h>
    #include <sys/mount.h>
    #include <CoreFoundation/CoreFoundation.h>
    #include <ifaddrs.h>
    #include <sys/socket.h>
    #include <arpa/inet.h>
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

void get_hostname(struct sysinfo *info) {
    char host[128] = "";
    char user[32] = "";
    get_command_out("uname -n", host);
    get_command_out("whoami", user);
    snprintf(info->hostname, sizeof(info->hostname), "%s@%s", user, host);
}

void get_os(struct sysinfo *info) {
#ifdef __linux__
        
#elif defined(__APPLE__)
    char buffer[32];
    get_command_out("sw_vers -productVersion", buffer);
    snprintf(info->os, sizeof(info->os), "macOS %s", buffer);
#endif
}

void get_kernel(struct sysinfo *info) {
    get_command_out("uname -sr", info->kernel);
}

void get_uptime(struct sysinfo *info) {
double time = 0;
#ifdef __linux__

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
    char cpu[32];
    char cores[4];
#ifdef __linux__

#elif defined(__APPLE__)
    get_command_out("sysctl -n machdep.cpu.brand_string", cpu);
    get_command_out("sysctl -n hw.physicalcpu", cores);
#endif
    snprintf(info->cpu, sizeof(info->cpu), "%s (%s cores)", cpu, cores);
}

void get_gpu(struct sysinfo *info) {
    char gpu[64] = "";
    int cores = 0;
#ifdef __linux__
    
#elif defined(__APPLE__)
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
#endif
    snprintf(info->gpu, sizeof(info->gpu), "%s (%d cores)", gpu, cores);
}

void bytes_to_barinfo(int64_t used_bytes, int64_t total_bytes, char *buffer, int size) {
    double mb_used = used_bytes / 1024 / 1024;
    double gb_used = mb_used / 1024;
    
    double mb_size = total_bytes / 1024 / 1024;
    double gb_size = mb_size / 1024;

    double temp = ((double)used_bytes / total_bytes);
    int percent = temp * 100;
    char bar_used[10] = "";
    char bar_avail[10] = "";
    char mem_used[8] = "";
    if (gb_used > 0) {
        snprintf(mem_used, sizeof(mem_used), "%.1fGB", gb_used);
    } else {
        snprintf(mem_used, sizeof(mem_used), "%.1fMB", mb_used);
    }

    char mem_size[8] = "";
    if (gb_size > 0) {
        snprintf(mem_size, sizeof(mem_size), "%.1fGB", gb_size);
    } else {
        snprintf(mem_size, sizeof(mem_size), "%.1fMB", mb_size);
    }

    int i = 0;
    for (; i < (percent / 10); i++) {
        bar_used[i] = '#';
    }
    int j = 0;
    for (; i < 10; i++) {
        bar_avail[j] = '-';
        j++;
    }

    snprintf(buffer, size, "(%s%s)   %s/%s", bar_used, bar_avail, mem_used, mem_size);
}

void get_mem(struct sysinfo *info) {
#ifdef __linux__
    
#elif defined(__APPLE__)
    int64_t total_bytes;
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
    
    int64_t used_bytes = (
        (uint64_t)container.active_count +
        (uint64_t)container.wire_count +
        (uint64_t)container.compressor_page_count
    ) * page_size;
#endif

    bytes_to_barinfo(used_bytes, total_bytes, info->memory, sizeof(info->memory));
}

void get_swap(struct sysinfo *info) {
#ifdef __linux__
    
#elif defined(__APPLE__)
    struct xsw_usage swap;
    size_t size = sizeof(struct xsw_usage);
    sysctlbyname("vm.swapusage", &swap, &size, NULL, 0);
    bytes_to_barinfo(swap.xsu_used, swap.xsu_total, info->swap, sizeof(info->swap));
#endif
}

void get_disk(struct sysinfo *info, char *path) {
#ifdef __linux__

#elif defined(__APPLE__)
    struct statfs disk_buffer;
    statfs(path, &disk_buffer);
    int64_t used_bytes = (disk_buffer.f_blocks - disk_buffer.f_bfree) * disk_buffer.f_bsize;
    int64_t total_bytes = disk_buffer.f_blocks * disk_buffer.f_bsize;
#endif
    char buffer[256] = "";
    bytes_to_barinfo(used_bytes, total_bytes, buffer, sizeof(buffer));
    snprintf(info->disk_info, sizeof(info->disk_info), "%s", buffer);
    snprintf(info->disk, sizeof(info->disk), "(%s)", path);
}

void get_locale(struct sysinfo *info) {
#ifdef __linux__

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
#ifdef __linux__

#elif defined(__APPLE__)
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
#ifdef __linux__

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
#endif
    snprintf(info->display, sizeof(info->display), "%s %s %dhz", display, resolution, refresh_rate);
}

void get_shell(struct sysinfo *info) {
    get_command_out("echo $SHELL", info->shell);
}


uint32_t get_dirs_in_dir(DIR *dir) {
    uint32_t num_elements = 0;

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
            ++num_elements;
        }
    }

    return num_elements;
}

void get_packages(struct sysinfo *info) {
    //uint64_t packages = 0;
#ifdef __linux__

#endif
    const char* brew_prefix = getenv("HOMEBREW_PREFIX");

    if (brew_prefix != NULL) {
        strcpy(info->package_man, "brew");

        uint32_t casks = 0;
        uint32_t formulae = 0;
        char path[sizeof(brew_prefix) + 32] = "";
        snprintf(path, sizeof(path), "%s/Caskroom", brew_prefix);
        DIR *dir = opendir(path);
        casks += get_dirs_in_dir(dir);
        closedir(dir);

        snprintf(path, sizeof(path), "%s/Cellar", brew_prefix);
        dir = opendir(path);
        formulae += get_dirs_in_dir(dir);
        closedir(dir);

        snprintf(info->package, sizeof(info->package), "%d formulae, %d casks", formulae, casks);
    }
}

void get_battery(struct sysinfo *info) {
#ifdef __linux__

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
}