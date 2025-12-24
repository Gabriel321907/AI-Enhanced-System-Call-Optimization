#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

#define CALLS 200
#define CSV_FILE "syscall_data.csv"

long get_time_ns() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000000000L + ts.tv_nsec;
}

const char* syscall_name(int type) {
    switch (type) {
        case 0: return "read";
        case 1: return "write";
        case 2: return "stat";
        default: return "unknown";
    }
}

int main() {
    FILE *csv;
    int i;
    pid_t pid = getpid();

    csv = fopen(CSV_FILE, "w");
    if (!csv) {
        perror("CSV file open failed");
        return 1;
    }

    
    fprintf(csv, "timestamp,pid,syscall,exec_time\n");

    printf("Collecting system call data...\n");

    for (i = 0; i < CALLS; i++) {
        int call_type = i % 3;
        long start, end, exec_time;
        long timestamp;

        timestamp = time(NULL); 
        start = get_time_ns();

        if (call_type == 0) {
            int fd = open("sample.txt", O_RDONLY | O_CREAT, 0644);
            char buffer[64];
            read(fd, buffer, sizeof(buffer));
            close(fd);
        }
        else if (call_type == 1) {
            int fd = open("output.txt", O_WRONLY | O_CREAT | O_APPEND, 0644);
            write(fd, "Hello OS\n", 9);
            close(fd);
        }
        else {
            struct stat st;
            stat("sample.txt", &st);
        }

        end = get_time_ns();
        exec_time = (end - start) / 1000; 

        fprintf(csv, "%ld,%d,%s,%ld\n",
                timestamp,
                pid,
                syscall_name(call_type),
                exec_time);
    }

    fclose(csv);

    printf("CSV data collection complete.\n");
    printf("Output file: %s\n", CSV_FILE);

    return 0;
}
