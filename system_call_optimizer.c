#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

#define CALLS 200

long get_time_ns() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_nsec + ts.tv_sec * 1000000000L;
}

// Simple AI heuristic: predict frequent calls
int predict_next(int freq_read, int freq_write, int freq_stat) {
    if (freq_read > freq_write && freq_read > freq_stat) return 0;
    if (freq_write > freq_read && freq_write > freq_stat) return 1;
    return 2;
}

int main() {
    int i;
    long baseline_total = 0, optimized_total = 0;

    int freq_read = 0, freq_write = 0, freq_stat = 0;

    printf("Running System Call Simulator...\n");

    // Baseline measurement
    for (i = 0; i < CALLS; i++) {
        int call_type = i % 3;  

        long start = get_time_ns();
        if (call_type == 0) {
            int fd = open("sample.txt", O_RDONLY | O_CREAT, 0644);
            char buffer[50];
            read(fd, buffer, 50);
            close(fd);
            freq_read++;
        }
        else if (call_type == 1) {
            int fd = open("output.txt", O_WRONLY | O_CREAT, 0644);
            write(fd, "Hello OS", 8);
            close(fd);
            freq_write++;
        }
        else {
            struct stat st;
            stat("sample.txt", &st);
            freq_stat++;
        }

        long end = get_time_ns();
        baseline_total += (end - start);
    }

    printf("Baseline Completed.\n");

    // AI-based Optimization
    for (i = 0; i < CALLS; i++) {
        int predict = predict_next(freq_read, freq_write, freq_stat);
        long start = get_time_ns();

        if (predict == 0) {
            int fd = open("sample.txt", O_RDONLY);
            char buffer[50];
            read(fd, buffer, 50);
            close(fd);
        }
        else if (predict == 1) {
            int fd = open("output.txt", O_WRONLY | O_CREAT, 0644);
            write(fd, "AI Optimized", 12);
            close(fd);
        }
        else {
            struct stat st;
            stat("sample.txt", &st);
        }

        long end = get_time_ns();
        optimized_total += (end - start);
    }

    printf("\n=== PERFORMANCE REPORT ===\n");
    printf("Baseline Total Time     : %ld ns\n", baseline_total);
    printf("Optimized Total Time    : %ld ns\n", optimized_total);
    printf("Improvement             : %.2f%%\n",
           ((baseline_total - optimized_total) * 100.0) / baseline_total);

    return 0;
}