#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    int rc = fork();
    if (rc == 0) {
        printf("I'm the child before STDOUT_FILENO is clsoed.\n");
        close(STDOUT_FILENO);
        printf("I'm the child after STDOUT_FILENO is clsoed.\n");
    } else if (rc > 0) {
        int wc = wait(NULL);
        printf("I'm the parent.\nchild pid is %d\nwait() returns %d\n", rc, wc);
    } else {
        fprintf(stderr, "fork failed\n");
    }
}