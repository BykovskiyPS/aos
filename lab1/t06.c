#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

extern int errno;

// Создание разряженного файла

int main(int argc, char* argv[]){
    if(argc != 3){
        fprintf(stderr, "Not enougth arguments\n");
        exit(1);
    }
    
    int size = strtol(argv[2], NULL, 0);

    int f;
    if((f = creat(argv[1], 0600)) == -1){
        perror("Create");
        exit(1);
    }

    write(f, "a", 1);
    lseek(f, size, SEEK_SET);
    write(f, "a", 1);
    close(f);

    return 0;
}