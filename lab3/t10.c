#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

extern int errno;

void f(int sig){
    int i, j;
    printf("Handler begin\n");
    for(i = 0; i < 0x7fff; i++)
        for(j = 0; j < 0xffff; j++);
    printf("Handler end\n");
}

int main(){
    struct sigaction act;
    act.sa_handler = f;
    sigemptyset(&act.sa_mask);
    sigaddset(&act.sa_mask, SIGINT);
    act.sa_flags = 0;
    sigaction(SIGQUIT, &act, NULL);
    for(;;);
}