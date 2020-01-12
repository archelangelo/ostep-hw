#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char *argv[]) {
    close(STDOUT_FILENO);
    open("./p2.output", O_CREAT|O_WRONLY|O_TRUNC, S_IRWXU);
    int rc = fork();
    if (rc < 0) {
        fprintf(stderr, "fork failed\n");
    } else if (rc == 0) {
        for (int i = 0; i < 1000; i++) {
            printf("child: %d\n", i);
        }
    } else {
        for (int i = 0; i < 1000; i++) {
            printf("parent: %d\n", i);
        }
    }
}