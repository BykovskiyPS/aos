#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

extern int errno;

int i = 0;

void f(int sig){
    printf("Signal received\n");
    i = 0;
}

int main(){
    int pid, rc;
    signal(SIGQUIT, f);
    if(fork()){
        pid = wait(&rc);
        printf("Child process %d exit with 0x%x status\n", pid, rc);
    } else {
        for(i; i < 1000; i++){
            for(int j = 0; j < 1000; j++){
                for(int k = 0; k < 1000; k++);
            }
            printf("%d ", i);
        }
        exit(3);
    }
    return 0;
}