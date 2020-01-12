# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <sys/wait.h>
# include <signal.h>

#if 0

int main(int argc, char *argv[]) {
    int rc = fork();
    if (rc < 0) {
        fprintf(stderr, "fork failed\n");
    } else if (rc == 0) {
        printf("I'm child, now printing:\n");
        printf("Hello.\n");
    } else {
        printf("I'm parent printing without waiting.\n");
        wait(NULL);
        printf("I'm parent, now printing:\n");
        printf("Goodbye.\n");
    }
}

#else

// Without calling wait()
// Solution found in https://github.com/ahmedbilal/OSTEP-Solution/blob/master/Chapter%205/question3-yorwos.c

void sighandle(int signum) {
    printf("I'm parent, printing:\n");
    printf("Goodbye.\n");
}

int main(int argc, char *argv[]) {
    int rc = fork();
    if (rc < 0) {
        fprintf(stderr, "fork failed\n");
    } else if (rc == 0) {
        printf("I'm child, now printing:\n");
        printf("Hello.\n");
        kill(getppid(), SIGCONT);
    } else {
        printf("I'm parent printing without waiting.\n");
        signal(SIGCONT, sighandle);
        pause();
    }
}

#endif