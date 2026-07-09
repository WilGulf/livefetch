#include "stdio.h"
#include "string.h"
#include "unistd.h"
#include "stdlib.h"
#include "sys/sysctl.h"

int system(const char *command);

int main() {
    FILE* file = fopen("logos/rhel_old.txt", "r");
    char line[256];

    if (file != NULL) {
        for (int i = 0; i < 20; i++) {
            int k = 0;

            if (fgets(line, sizeof(line), file)) {
                int j = 0;
                while (line[j]) {
                    if (line[j] != '\n') {
                        printf("%c", line[j]);
                    } else {
                        k--;
                    }
                    k++;
                    j++;
                }
            }
            for (; k < 50; k++) {
                printf(" ");
            }
            
            printf("hostname");
            printf("\n");

            k = 0;

            usleep(100000);
        }
        fclose(file);
    }
}