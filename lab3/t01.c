#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

extern int errno;

void f(int sig){
    signal(SIGINT, SIG_DFL);
    printf("Signal received\n");
}

int main(){
    signal(SIGINT, f);
    for(;;);
}