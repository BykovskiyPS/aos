#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

extern int errno;

// r-------- 400

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

    int l;
    char string[] = "string\n";
    for(int i = 0; i < 2; i++){
        l = write(f, string, sizeof(string) - 1);
    }
    close(f);

    f = open(argv[1], O_RDONLY);
    if(f == -1){
        perror("Open on read");
        exit(1);
    }
    
    char buffer[1024];
    while(read(f, buffer, l) > 0){
        printf("%s", buffer);
        // write(1, buffer, l);
    }
    close(f);

    f = open(argv[1], O_RDWR);
    if(f == -1){
        perror("Open on write and read");
        exit(1);
    }
    return 0;
}