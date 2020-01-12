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