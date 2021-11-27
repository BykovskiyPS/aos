#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>

extern int errno;

// Выполнить команду shell с помощью fork-exec

int main(int argc, char* argv[], char* envp[]){
    if(argc < 2){
        fprintf(stderr, "Bad number arguments %s\n", argv[0]);
        exit(1);
    }
    int rc;
    if(fork()){
        wait(&rc);
        exit(0);
    } else {
        execvp(argv[1], argv+1);
        perror(argv[1]);
        exit(1);
    }
    return 0;
}