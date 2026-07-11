#ifndef SYSINFO_H
#define SYSINFO_H

struct sysinfo {
    char hostname[256];
    char os[256];
    char kernel[256];
    char uptime[256];
    int package;
    char shell[256];
    char display[256];
    char terminal[256];
    char cpu[256];
    char gpu[256];
    int memory_max;
    int memory_use;
    int swap_max;
    int swap_use;
    int disk_max;
    int disk_use;
    char local_ip[256];
    int battery;
    char locale[256];
};

void get_hostname(struct sysinfo *info);
void get_os(struct sysinfo *info);
void get_kernel(struct sysinfo *info);

void get_uptime(struct sysinfo *info);

void get_cpu(struct sysinfo *info);
void get_gpu(struct sysinfo *info);

#endif