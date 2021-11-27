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

int main(){
    struct flock lock;
    int f, l;
    char* prefix;
    char buf[80];
    if(f = open("file", O_RDWR | O_CREAT, 0600) == -1){
        perror("file");
        exit(1);
    }
    if(fork()) prefix = "father: ";
    else prefix = "son: ";
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;
    if(fcntl(f, F_SETLKW, &lock) == -1){
        perror("file");
        exit(1);
    }
    while((l = read(0, buf, 79)) > 0){
        buf[l] = '\0', printf("%s%s", prefix, buf);
        sleep(1);
    }
    lock.l_type = F_UNLCK;
    fcntl(f, F_SETLK, &lock);
    close(f);
    wait(NULL);
    exit(0);
}