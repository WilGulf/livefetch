#include "sysinfo.h"
#include "string.h"
#include "stdio.h"

#ifdef __linux__
    #include "stdlib.h"
    #include "sys/sysctl.h"
#elif defined(__APPLE__)
    #include "stdlib.h"
    #include "sys/sysctl.h"
    #include "time.h"
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
    char host[32];
    char user[32];
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
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    double time = ts.tv_sec + ts.tv_nsec / 1e9;
    
    int64_t seconds = time;
    int64_t minutes = seconds / 60;
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
    char gpu[32];
#ifdef __linux__

#elif defined(__APPLE__)
    
#endif
}

void get_memort(struct sysinfo *info) {
    
}