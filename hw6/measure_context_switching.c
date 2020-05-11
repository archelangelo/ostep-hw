#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <limits.h>
#include <sched.h>

#define N_ITERATIONS 1000

long calculate_time_in_us(struct timeval tv0, struct timeval tv1) {
    return (tv1.tv_sec - tv0.tv_sec) * 1000000 + tv1.tv_usec - tv0.tv_usec;
}

int main(int argc, char *argv[]) {
    printf("Parent before everything: pid %d cpu %d\n", getpid(), sched_getcpu());

    // Check the arguments
    if (argc < 2) {
        printf("Usage: measure_context_switching <CPU>\n");
        exit(EXIT_FAILURE);
    }

    // Set cpu affinity
    int cpu_selection = atoi(argv[1]);
    cpu_set_t cpu_set;
    CPU_ZERO(&cpu_set);
    CPU_SET(cpu_selection, &cpu_set);
    if (sched_setaffinity(0, sizeof(cpu_set), &cpu_set) != 0) {
        printf("Failed to set affinity.\n");
        exit(EXIT_FAILURE);
    }
    printf("Parent after setting affinity: pid %d cpu %d\n", getpid(), sched_getcpu());

    // Open pipes
    int pfdp[2], pfdc[2];
    if (pipe(pfdp) < 0 || pipe(pfdc) < 0) {
        fprintf(stderr, "Piping failed.\n");
        exit(EXIT_FAILURE);
    }

    // Fork
    int rc = fork();
    if (rc == 0) {
        // Child
        printf("Child: pid %d cpu %d\n", getpid(), sched_getcpu());
        close(pfdp[1]);
        close(pfdc[0]);
        char buff;
        while (read(pfdp[0], &buff, 1) > 0) {
            write(pfdc[1], &buff, 1);
        }
        close(pfdp[0]);
        close(pfdc[1]);
    } else if (rc > 0) {
        // Parent
        printf("Parent: pid %d cpu %d\n", getpid(), sched_getcpu());
        close(pfdc[1]);
        close(pfdp[0]);
        char buff;
        long total_time = 0, shortest_time = LONG_MAX;
        struct timeval tv0, tv1;
        for (int i = 0; i < N_ITERATIONS; i++) {
            write(pfdp[1], &buff, 1);
            gettimeofday(&tv0, NULL);
            if (read(pfdc[0], &buff, 1) > 0) {
                // Calculate time
                gettimeofday(&tv1, NULL);
                long elapsed_time = calculate_time_in_us(tv0, tv1);
                total_time += elapsed_time;
                shortest_time = elapsed_time < shortest_time ? elapsed_time : shortest_time;
            } else {
                fprintf(stderr, "Error: read from child pipe failed.\n");
                break;
            }
        }
        printf("Total time for %d iterations is %ld\nAverage is %f\n", N_ITERATIONS, total_time, (double)total_time/N_ITERATIONS);
        printf("Shortest time for an iteration is %ld\n", shortest_time);
        close(pfdc[0]);
        close(pfdp[1]);

    } else {
        // Fork failed
        fprintf(stderr, "Forking failed.\n");
        exit(EXIT_FAILURE);
    }
}