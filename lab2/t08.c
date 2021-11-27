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

// Читаемая процессами информация выводится в свои файлы

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
    if(fork()){
        int father = creat("father", 0600);
        while((l = read(f, buffer, 10)) != 0){
            write(father, buffer, l);
        }
        close(father);
        wait(&rc);
        printf("son return status=%d\n", rc);
        printf("===================This is father===================\n");
        // if((seek = lseek(father, 0, SEEK_SET)) == -1){
        //     perror("lseek");
        // }
        father = open("father", O_RDONLY);
        while((l = read(father, buffer, 10)) != 0){
            write(1, buffer, l);
        }
        close(father);
        close(f);
    } else {
        int son = creat("son", 0600);
        while((l = read(f, buffer, 10)) != 0){
            write(son, buffer, l);
        }
        close(son);
        printf("===================This is son===================\n");
        // if((seek = lseek(son, 0, SEEK_SET)) == -1){
        //     perror("lseek");
        // }
        son = open("son", O_RDONLY);
        while((l = read(son, buffer, 10)) != 0){
            write(1, buffer, l);
        }
        close(son);
        exit(0);
    }
    return 0;
}