#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>

extern int errno;

// вывод идентификаторов процессов

int main(int argc, char* argv[]){
    int pid, code;
    if(fork()){
        // parent
        printf("I'm parent with PID=%d, PPID=%d, GPID=%d\n", getpid(), getppid(), getpgrp()) ;
        pid = wait(&code);
        printf("Child process %d was done with code=0x%x\n", pid, code);
        exit(0);
    } else {
        // child
        printf("I'm child with PID=%d, PPID=%d, GPID=%d\n", getpid(), getppid(), getpgrp());
        exit(10);
    }
    return 0;
}