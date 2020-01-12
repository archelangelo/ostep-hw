#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    int x = 99;
    int rc = fork();
    if (rc < 0) {
        fprintf(stderr, "fork failed\n");
        exit(1);
    } else if (rc == 0) {
        // child
        printf("hello, I am child x value before change is %d\n", x);
        x = 4;
        printf("hello, I am child and the x value is now %d\n", x);
    } else {
        // parent
        printf("hello, I am parent x value before change is %d\n", x);
        x = 39;
        printf("hello, I am parent and the x value is now %d\n", x);
    }
}