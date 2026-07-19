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
    char memory[256];
    char swap[256];
    char disk_info[256];
    char disk[16];
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

void get_display(struct sysinfo *info);

void get_mem(struct sysinfo *info);
void get_swap(struct sysinfo *info);
void get_disk(struct sysinfo *info, char *path);

void get_local_ip(struct sysinfo *info);
void get_locale(struct sysinfo *info);

#endif