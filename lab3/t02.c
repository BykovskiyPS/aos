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
    printf("SIgnal received\n");
}

int main(){
    struct sigaction act;
    act.sa_handler = f;
    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_RESETHAND;
    sigaction(SIGINT, &act, NULL);
    for(;;);
}