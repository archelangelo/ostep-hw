#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#if 0
// execvp
int main(int argc, char *argv[]) {
    int rc = fork();
    if (rc == 0) {
        char *myargs[2];
        myargs[0] = strdup("ls");
        myargs[1] = NULL;
        execvp(myargs[0], myargs);
    } else if (rc > 0) {
        wait(NULL);
        printf("I'm the parent.\n");
    } else {
        fprintf(stderr, "fork failed\n");
    }
}

#elif 0
// execv
int main(int argc, char *argv[]) {
    int rc = fork();
    if (rc == 0) {
        char *myargs[2];
        myargs[0] = strdup("ls");
        myargs[1] = NULL;
        execv(strdup("/bin/ls"), myargs);
    } else if (rc > 0) {
        wait(NULL);
        printf("I'm the parent.\n");
    } else {
        fprintf(stderr, "fork failed\n");
    }
}

#elif 0
// execvpe
int main(int argc, char *argv[]) {
    int rc = fork();
    if (rc == 0) {
        char *myargs[2];
        char *myenv[] = {NULL};
        myargs[0] = strdup("ls");
        myargs[1] = NULL;
        execvpe(myargs[0], myargs, myenv);
    } else if (rc > 0) {
        wait(NULL);
        printf("I'm the parent.\n");
    } else {
        fprintf(stderr, "fork failed\n");
    }
}

#elif 0
// execve
int main(int argc, char *argv[]) {
    int rc = fork();
    if (rc == 0) {
        char *myargs[2];
        char *myenv[] = {NULL};
        myargs[0] = strdup("ls");
        myargs[1] = NULL;
        execve("/bin/ls", myargs, myenv);
    } else if (rc > 0) {
        wait(NULL);
        printf("I'm the parent.\n");
    } else {
        fprintf(stderr, "fork failed\n");
    }
}

#elif 1
// execlp
int main(int argc, char *argv[]) {
    int rc = fork();
    if (rc == 0) {
        char *myargs[3];
        char *myenv[] = {NULL};
        myargs[0] = strdup("ls");
        myargs[1] = strdup("./");
        myargs[2] = NULL;
        execlp(myargs[0], myargs[0], myargs[1], myargs[2]);
    } else if (rc > 0) {
        wait(NULL);
        printf("I'm the parent.\n");
    } else {
        fprintf(stderr, "fork failed\n");
    }
}

#endif