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
    int l;
    char* prefix;
    char buf[80];
    if(fork()) prefix = "father: ";
    else prefix = "son: ";
    while((l = read(0, buf, 79)) > 0){
        buf[l] = '\0', printf("%s%s", prefix, buf);
        sleep(1);
    }
    wait(NULL);
    exit(0);
}