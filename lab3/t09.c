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

void f(int sig){
    printf("Received ALARM\n");
}

int main(){
    // signal(SIGALRM, f); // without handler
    int pid, rc;
    if(fork()){
        pid = wait(&rc);
        printf("Child process %d exit with 0x%x status\n", pid, rc);
    } else {
        alarm(2);
        pause();
        exit(3);
    }
    return 0;
}