Homework 5

1. Write a program that calls fork(). Before calling fork(), have the main process access a variable (e.g., x) and set its value to something (e.g., 100). What value is the variable in the child process? What happens to the variable when both the child and parent change the value of x?

Since the child will just copy the memory of parent, x for parent and child are both 100 to bigin with. Then when you change x, the two x's occupy different memory and thus will be independent from each other.
```c
// p1.c
# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>

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
```
```
> ./p1.out
hello, I am parent x value before change is 99
hello, I am parent and the x value is now 39
hello, I am child x value before change is 99
hello, I am child and the x value is now 4
```

2. Write a program that opens a file (with the open() system call) and then calls fork() to create a new process. Can both the child and parent access the file descriptor returned by open()? What happens when they are writing to the file concurrently, i.e., at the same time?

I imagine they can both access the file descriptor. And when they write they messages are going to be interlaced.

```c
// p2.c
# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <fcntl.h>

int main(int argc, char *argv[]) {
    close(STDOUT_FILENO);
    open("./p2.out", O_CREAT|O_WRONLY|O_TRUNC, S_IRWXU);
    int rc = fork();
    if (rc < 0) {
        fprintf(stderr, "fork failed\n");
    } else if (rc == 0) {
        printf("I am child, now writing to the file\n");
    } else {
        printf("I am parent. I am not happy because my child interrupts me.\n");
    }
}
```
```
> ./p2.out
> cat p2.output
I am parent. I am not happy because my child interrupts me.
I am child, now writing to the file
```
So it seems that the outputs are not interlaced. I also tried outputing multiple line of the same thing but it doesn't get interlaced. Now what if I tried even more outputs...

```c
// p2.c for loop outputs
# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <fcntl.h>

int main(int argc, char *argv[]) {
    close(STDOUT_FILENO);
    open("./p2.out", O_CREAT|O_WRONLY|O_TRUNC, S_IRWXU);
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
```

Yes it turns out the outputs do get interlaced, but quite rarely. The stream broke twice for 1000 outputs. See these part of the output:


```
parent: 0
parent: 1
parent: 2
...
parent: 688
parent: 689
parent: 690
parent: 69child: 0
child: 1
child: 2
child: 3
...
child: 379
child: 380
child: 381
chil1
parent: 692
parent: 693
parent: 694
...
parent: 997
parent: 998
parent: 999
d: 382
child: 383
child: 384
child: 385
...
child: 997
child: 998
child: 999
```

3. Write another program using fork(). The child process should print “hello”; the parent process should print “goodbye”. You should try to ensure that the child process always prints first; can you do this without calling wait() in the parent?

- Using `wait()`.
```c
# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <sys/wait.h>

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
```
```
> ./p3.out
I'm parent printing without waiting.
I'm child, now printing:
Hello.
I'm parent, now printing:
Goodbye.
```

- Without using `wait()`, solution from [ahmedbilal/OSTEP-Solution](https://github.com/ahmedbilal/OSTEP-Solution/blob/master/Chapter%205/question3-yorwos.c).
```c
# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <sys/wait.h>

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
```
```
./p3.out
I'm parent printing without waiting.
I'm child, now printing:
Hello.
I'm parent, printing:
Goodbye.
```

4. Write a program that calls fork() and then calls some form of exec() to run the program /bin/ls. See if you can try all of the variants of exec(), including (on Linux) execl(), execle(), execlp(), execv(), execvp(), and execvpe(). Why do you think there are so many variants of the same basic call?

According to the [man page](https://linux.die.net/man/3/execvpe):
> The exec() family of functions replaces the current process image with a new process image. The functions described in this manual page are front-ends for execve(2).

So all the variants are just based on `execve`.

All these variants are `exec` followed by `v`, `l`, `p`, `e`.

- `v`
Pass command line arguments as an array.

- `l`
Pass command line arguments in the parameter list.

- `p`
Use the `PATH` environment variable to search for the file to be executed. e.g. no need to specify `/bin/ls` path for the executable, just need to tell the syscall `ls`.

- `e`
Pass environment variables as a string array. e.g. `"KEY=SOMEKEY"`.

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

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
```

5. Now write a program that uses wait() to wait for the child process to finish in the parent. What does wait() return? What happens if you use wait() in the child?

`wait()` returns the process id that returns. In the child process's case the `wait()` returns `-1`.

```c
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
```
```
> ./p5.out
I'm the child.
My pid is 25309
wait() returns -1
I'm the parent.
child pid is 25309
wait() returns 25309
```

6. Write a slight modification of the previous program, this time using waitpid() instead of wait(). When would waitpid() be useful?

`waitpid()` not only allows for waiting for a process specified by its pid, but also finer granularity of control. For example, you can wait for a certain process's certain activity (e.g. CONTINUE, STOP, etc.)

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    int rc = fork();
    if (rc == 0) {
        printf("I'm the child.\nMy pid is %d\n", getpid());
        int wc = wait(NULL);
        printf("wait() returns %d\n", wc);
    } else if (rc > 0) {
        int wc = waitpid(rc, NULL, 0);
        printf("I'm the parent.\nchild pid is %d\nwait() returns %d\n", rc, wc);
    } else {
        fprintf(stderr, "fork failed\n");
    }
}
```
```
> ./p6.out
I'm the child.
My pid is 26432
wait() returns -1
I'm the parent.
child pid is 26432
wait() returns 26432
```

7. Write a program that creates a child process, and then in the child closes standard output (STDOUT FILENO). What happens if the child calls printf() to print some output after closing the descriptor?

After the child closes the `STDOUT_FILENO` descriptor, there is not output from child.

```c
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
```
```
> ./p7.out
I'm the child before STDOUT_FILENO is clsoed.
I'm the parent.
child pid is 26970
wait() returns 26970
```

8. Write a program that creates two children, and connects the standard output of one to the standard input of the other, using the pipe() system call.

How does the downstream process know that the pipeline stream has done? All parties have close their file descriptors for the stream to be done and the `read()` to return a negative value.

```c
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
```
```
> ./p8.out
I'm child 1 and I'm the downstream. pid 28017
I'm child 2 and I'm the upstream. pid 28018
This is a message to be tranmitted.
I wish the other side all the luck receiving it.
Done outputing
I'm the parent, wait() returned 28017
```