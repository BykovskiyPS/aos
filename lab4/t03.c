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
Модификация программы 2.
Добавление двунаправленной связи
*/

void reverse(char* buf, int l){
    l--;
    for(size_t i = 0; i < (l / 2); i++){
        char tmp = buf[i];
        buf[i] = buf[l - i - 1];
        buf[l - i - 1] = tmp;
    }
}

int main(){
    int p, l, k;
    char buf[80];
    int fd[2];
    int fd2[2];
    pipe(fd);
    pipe(fd2);
    if(p = fork()){
        close(fd[1]);
        close(fd2[0]);
        while((l = read(fd[0], buf, 80)) > 0){
            // write(1, buf, l);
            reverse(buf, l);
            write(fd2[1], buf, l);
        }
        close(fd[0]);
        close(fd2[1]);
        wait(NULL);
        exit(0);
    } else {
        close(fd[0]);
        close(fd2[1]);
        while((l = read(0, buf, 80)) > 0){
            write(fd[1], buf, l);
            k = read(fd2[0], buf, 80);
            write(1, buf, k);
        }
        close(fd[1]);
        close(fd2[0]);
        exit(0);
    }
}