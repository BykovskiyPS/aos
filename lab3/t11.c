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

void blink_red(){
    write(1, "RED ", 4);
    sleep(1);
}

void blink_green(){
    write(1, "GREEN ", 6);
    sleep(1);
}

void f(int sig){
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGQUIT);
    sigprocmask(SIG_BLOCK, &mask, NULL);
    printf("Handler start\n");
    for(int i = 0; i < 0x7fff; i++)
        for(int j = 0; j < 0x7fff; j++);

    printf("Handler end\n");
}

int main(){
    int green;
    struct sigaction act;
    act.sa_handler = f;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    sigaction(SIGUSR1, &act, NULL);

    if(green = fork()){
        for(;;) {
            for(int i = 0; i < 3; i++)
                blink_red();
            printf("\n");
            kill(green, SIGUSR1);
            sigsuspend(&act.sa_mask);
        }
    } else {
        int red = getppid();
        for(;;) {
            sigsuspend(&act.sa_mask);
            for(int i = 0; i < 3; i++)
                blink_green();
            printf("\n"); 
            kill(red, SIGUSR1);
        }
    }
}
