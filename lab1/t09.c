#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

extern int errno;

// modifed copy from file1 to file2

void copy(int from, int out){
    int l;
    char buffer[4096];
    while((l = read(from, buffer, 4096))){
        write(out, buffer, l);
    }
}

int main(int argc, char* argv[]){
    if(argc > 3){
        fprintf(stderr, "Too many arguments\n");
        exit(1);
    }
    if(argc > 2){
        close(1);
        if(creat(argv[2], 0777) == -1){
            perror(argv[2]);
            exit(1);
        }
    }
    if(argc > 1){
        close(0);
        if(open(argv[1], O_RDONLY) == 1){
            perror(argv[1]);
            exit(1);
        }
    }
    copy(0, 1);
    return 0;
}