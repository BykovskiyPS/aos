#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

extern int errno;

int main(){
    int pid, rc;
    if(pid = fork()){
        kill(pid, SIGUSR1);
        pid = wait(&rc);
        printf("Child process %d exit with 0x%x status\n", pid, rc);
    } else {
        for(int i = 0; i < 1000; i++){
            for(int j = 0; j < 1000; j++){
                for(int k = 0; k < 1000; k++);
            }
            printf("%d ", i);
        }
        exit(3);
    }
    return 0;
}