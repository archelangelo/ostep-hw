#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char *argv[]) {
    int pfd[2];
    if (pipe(pfd) < 0) {
        fprintf(stderr, "Piping failed.");
        exit(1);
    }
    int rc1 = fork();
    if (rc1 == 0) {
        // Child 1 Downstream
        printf("I'm child 1 and I'm the downstream. pid %d\n", getpid());
        close(pfd[1]);
        char buf;
        while (read(pfd[0], &buf, 1) > 0) {
            write(STDOUT_FILENO, &buf, 1);
        }
        printf("Done outputing\n");
        close(pfd[0]);
    } else {
        // Parent
        int rc2 = fork();
        if (rc2 == 0) {
            // Child 2 Upstream
            printf("I'm child 2 and I'm the upstream. pid %d\n", getpid());
            close(pfd[0]);
            char buf[] = "This is a message to be tranmitted.\nI wish the other side all the luck receiving it.\n";
            write(pfd[1], &buf[0], strlen(buf));
            close(pfd[1]);
        } else {
            // Parent
            close(pfd[0]);
            close(pfd[1]);
            int wc = waitpid(rc1, NULL, 0);
            printf("I'm the parent, wait() returned %d\n", wc);
        }
    }
}