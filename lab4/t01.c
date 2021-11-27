#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

extern int errno;

int main(){
    int p;
    char buf1[2];
    char buf2[2];
    int fd[2];
    int fd2[2];
    pipe(fd);
    pipe(fd2);
    if(p = fork()){
        // читает из 2го
        // пишет в 1й
        close(fd[0]);
        close(fd2[1]);

        write(fd[1], "aa", 2);
        read(fd2[0], buf1, 2);
        printf("%s\n", buf1); // output
        wait(NULL);
        printf("child successful completed\n");
        
        close(fd[0]);
        close(fd2[1]);
        exit(0);
    } else {
        // пишет во 2й
        // читает из 1го
        close(fd[1]);
        close(fd2[0]);

        write(fd2[1], "bb", 2);
        read(fd[0], buf2, 2);
        printf("%s\n", buf2); // output

        close(fd[1]);
        close(fd2[0]);
        exit(0);
    }
}