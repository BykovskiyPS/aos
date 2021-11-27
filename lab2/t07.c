#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

extern int errno;

int main(int argc, char* argv[]){
    int pid, code;
    if(fork()){
        // parent
        printf("I'm parent with PID=%d, PPID=%d, GPID=%d\n", getpid(), getppid(), getpgrp()) ;
        pause();
        exit(0);
    } else {
        // child
        setpgrp();
        printf("I'm child after change GPID with PID=%d, PPID=%d, GPID=%d\n", getpid(), getppid(), getpgrp());
        pause();
        exit(0);
    }
    return 0;
}