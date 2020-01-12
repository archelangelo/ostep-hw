#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    int rc = fork();
    if (rc == 0) {
        printf("I'm the child.\nMy pid is %d\n", getpid());
        int wc = wait(NULL);
        printf("wait() returns %d\n", wc);
    } else if (rc > 0) {
        int wc = wait(NULL);
        printf("I'm the parent.\nchild pid is %d\nwait() returns %d\n", rc, wc);
    } else {
        fprintf(stderr, "fork failed\n");
    }
}