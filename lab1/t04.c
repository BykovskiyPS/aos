#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

extern int errno;

// rw------- 600

int main(int argc, char* argv[]){
    if(argc != 3){
        fprintf(stderr, "Not enougth arguments\n");
        exit(1);
    }
    
    int mode = strtol(argv[2], NULL, 8);

    int f = creat(argv[1], mode);
    if(f == -1){
        perror("Create");
        exit(1);
    }

    char string[] = "string\n";
    write(f, string, sizeof(string) - 1);
    lseek(f, 0, SEEK_SET);

    char buffer[1024];
    if(read(f, buffer, 7) == -1){
        perror("Read");
        exit(1);
    }
    close(f);

    return 0;
}