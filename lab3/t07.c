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
        kill(pid, SIGCONT);
        pid = wait(&rc);
        printf("Child process %d exit with 0x%x status\n", pid, rc);
    } else {
        pause();
        exit(3);
    }
    return 0;
}