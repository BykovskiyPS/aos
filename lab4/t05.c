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
    int fd[2];
    pipe(fd);
    if(fork()){
        close(fd[1]);
        close(0);
        dup(fd[0]);
        close(fd[0]);
        execlp("wc", "wc", "-w", NULL);
        perror("wc");
        exit(0);
    } else {
        close(fd[0]);
        close(1);
        dup(fd[1]);
        close(fd[1]);
        execlp("who", "who", NULL);
        perror("who");
        exit(0);
    }
    exit(0);
}