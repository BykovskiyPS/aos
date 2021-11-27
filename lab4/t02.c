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
Родительский процесс читает из канала в цикле,
пока не встретится конец файла.
Дочерний процесс записывает в канал из стандартного
входного потока
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
        close(fd[0]);
        while((l = read(0, buf, 80)) > 0){
            write(fd[1], buf, l);
        }
        close(fd[1]);
        exit(0);
    }
}