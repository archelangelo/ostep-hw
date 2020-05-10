#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>

#define N_ITERATION 100

int main(int argc, char *argv[]) {
    struct timeval tv0, tv;
    gettimeofday(&tv0, NULL);
    for (int i = 0; i < N_ITERATION; i++) {
        gettimeofday(&tv, NULL);
        long elapsed_time_us = (tv.tv_sec - tv0.tv_sec) * 1000000 + tv.tv_usec - tv0.tv_usec;
        // printf("Iteration %d: elapsed time is %ld us\n", i, elapsed_time_us);
        printf("%ld\n", elapsed_time_us);
    }
}