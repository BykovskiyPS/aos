#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>

extern int errno;

// /var/log/auth.log

// Читаемая процессами информация сразу выводится на экран

int main(int argc, char* argv[]){
    if(argc != 2){
        printf("Bad number arguments\n");
        exit(1);
    }
    int f, rc;
    if((f = open(argv[1], O_RDONLY)) == -1){
        perror("Bad file");
        exit(1);
    }
    char buffer[10];
    int l;
    fork();
    while((l = read(f, buffer, 10)) != 0){
        write(1, buffer, l);
    }
    return 0;
}