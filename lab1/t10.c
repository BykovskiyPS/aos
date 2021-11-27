#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

extern int errno;

// reverse file

int main(int argc, char* argv[]){
    if(argc != 2){
        fprintf(stderr, "Bad argumnets\n");
        exit(1);
    }
    int f;
    if((f = open(argv[1], O_RDONLY)) == -1){
        perror("Open");
        exit(1);
    }
    char c[1];
    for(lseek(f, 1, SEEK_END); lseek(f, -2, SEEK_CUR) != -1; read(f, c, 1), write(1, c, 1));
    close(f);
    return 0;
}