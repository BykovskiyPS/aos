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
    struct flock lock;
    int buf;
    if(argc != 2){
        fprintf(stderr, "Not enougth arguments\n");
        exit(1);
    }
    close(creat(argv[1], 0755));

    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;
    lock.l_type = F_WRLCK;

    if(fork()){
        if((f = open(argv[1], O_WRONLY)) == -1){
            perror(argv[1]);
            exit(1);
        }
        if(fcntl(f, F_SETLKW, &lock) == -1){
            perror("file");
            exit(1);
        }
        int i = 0;
        while(i < 50){
            buf = i;
            write(f, &buf, 4);
            i++;
        }
        lock.l_type = F_UNLCK;
        fcntl(f, F_SETLK, &lock);
        close(f);
        wait(NULL);
        exit(0);
    } else {
        char* buf;
        if((f = open(argv[1], O_RDONLY)) == -1){
            perror(argv[1]);
            exit(1);
        }
        lock.l_type = F_RDLCK;
        if(fcntl(f, F_SETLKW, &lock) == -1){
            perror("file");
            exit(1);
        }
        while((l = read(f, &buf, 4)) > 0){
            write(1, &buf, 4);
        }
        lock.l_type = F_UNLCK;
        fcntl(f, F_SETLK, &lock);
        close(f);
        exit(0);
    }
}