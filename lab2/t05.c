#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

extern int errno;

// родительский процесс не ожидает завершения порожденного процесса (zombie)

int main(int argc, char* argv[]){
    int pid, code;
    if(fork()){
        // parent
        printf("I'm parent with PID=%d, PPID=%d, GPID=%d\n", getpid(), getppid(), getpgrp()) ;
        // sleep(2);
        pause();
        system("ps");
        exit(0);
    } else {
        // child
        printf("I'm child with PID=%d, PPID=%d, GPID=%d\n", getpid(), getppid(), getpgrp());
        exit(0);
    }
    return 0;
}