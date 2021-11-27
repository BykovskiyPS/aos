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

int main(int argc, char* argv[]){
    int f, l;
    char buf[4096];
    printf("Before open\n");
    // if((f = open(argv[1], O_RDONLY)) == -1){
    //     perror(argv[1]);
    //     exit(1);
    // }
    if((f = open(argv[1], O_RDONLY | O_NDELAY)) == -1){
        perror(argv[1]);
        exit(1);
    }
    printf("After open\n");
    while((l = read(f, buf, 4096)) > 0){
        write(1, buf, l);
    }
    close(f);
    exit(0);
}