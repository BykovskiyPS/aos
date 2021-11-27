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

// Общий файл для чтения открывается в каждом из процессов

int main(int argc, char* argv[]){
    if(argc != 2){
        printf("Bad number arguments\n");
        exit(1);
    }
    int f;
    if((f = open(argv[1], O_RDONLY)) == -1){
        perror("Bad file");
        exit(1);
    }
    char buffer[10];
    int l, rc;
    if(fork()){
        int f;
        if((f = open(argv[1], O_RDONLY)) == -1){
            perror("Bad file");
            exit(1);
        }
        int father = creat("father", 0600);
        while((l = read(f, buffer, 10)) != 0){
            write(father, buffer, l);
        }
        close(father);
        close(f);
        wait(&rc);
        printf("===================This is father===================\n");
        // if((seek = lseek(father, 0, SEEK_SET)) == -1){
        //     perror("lseek");
        // }
        father = open("father", O_RDONLY);
        while((l = read(father, buffer, 10)) != 0){
            write(1, buffer, l);
        }
        close(father);
    } else {
        int s;
        if((s = open(argv[1], O_RDONLY)) == -1){
            perror("Bad file");
            exit(1);
        }
        int son = creat("son", 0600);
        while((l = read(s, buffer, 10)) != 0){
            write(son, buffer, l);
        }
        close(son);
        close(s);
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