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
    int l, f;
    int buf;
    if(argc != 2){
        fprintf(stderr, "Not enougth arguments\n");
        exit(1);
    }
    close(creat(argv[1], 0755));

    if(fork()){
        if((f = open(argv[1], O_WRONLY)) == -1){
            perror(argv[1]);
            exit(1);
        }
        int i = 0;
        while(i < 50){
            buf = i;
            write(f, &buf, 4);
            i++;
        }
        wait(NULL);
        close(f);
        exit(0);
    } else {
        if((f = open(argv[1], O_RDONLY)) == -1){
            perror(argv[1]);
            exit(1);
        }
        while((l = read(f, &buf, 4)) > 0){
            write(1, &buf, 4);
        }
        close(f);
        exit(0);
    }
}