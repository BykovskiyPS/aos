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

/*
п.2, но создается процесс и вызывается команда с помощью exec
*/

int main(){
    int p, l;
    char buf[80];
    int fd[2];
    pipe(fd);
    if(p = fork()){
        close(fd[1]);
        while((l = read(fd[0], buf, 80)) > 0){
            write(1, buf, l);
        }
        close(fd[0]);
        wait(NULL);
        exit(0);
    } else {
        dup2(fd[1], 1);
        close(fd[0]);
        close(fd[1]);
        execl("t04child", "t04child", NULL);
        perror("exec");
        exit(1);
    }
}