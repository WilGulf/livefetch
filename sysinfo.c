#include "sys/sysctl.h"
#include "sysinfo.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"

const char *getCommandOutput(char *cmd[128])
{

    FILE *fp;
    char path[1035];

    fp = popen(cmd, "r");
    if (fp == NULL)
    {
        return "";
    }

    while (fgets(path, sizeof(path), fp) != NULL) {}

    pclose(fp);

    char buffer[128];

    int i = 0;
    for (; i < strlen(path); i++) {
        if (path[i] == '\n') {
            break;
        }
        buffer[i] = path[i];
    }
    

    return path;
}

const char *getHostname() {
    return "wilgulf@macbook";
}

const char *getKernel() {
    return "darwin";
}