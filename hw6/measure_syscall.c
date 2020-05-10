#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>

#define N_ITERATION 10000

int main(int argc, char *argv[]) {
    struct timeval tv0, tv1;
    gettimeofday(&tv0, NULL);
    for (int i = 0; i < N_ITERATION; i++) {
        read(STDIN_FILENO, NULL, 0);
    }
    gettimeofday(&tv1, NULL);
    long elapsed_time_us = (tv1.tv_sec - tv0.tv_sec) * 1000000 + tv1.tv_usec - tv0.tv_usec;
    double time_per_iteration = (double) elapsed_time_us / N_ITERATION;
    printf("Total time for %d iterations is %ld us\nTime per iteration is %f us\n", N_ITERATION, elapsed_time_us, time_per_iteration);
}