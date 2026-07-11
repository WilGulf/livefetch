#include "sysinfo.h"
#include "string.h"
#include "stdio.h"

#ifdef __linux__
    #include "stdlib.h"
    #include "sys/sysctl.h"
#elif defined(__APPLE__)
    #include "stdlib.h"
    #include "sys/sysctl.h"
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