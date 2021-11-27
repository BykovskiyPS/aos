#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>

extern int errno;

void f(int sig){
    int pid, rc;
    pid = wait(&rc);
    printf("My son %d with code 0x%x exited\n", pid, rc);
}

int main(){
    int p, f1, n;
    char status[256], buf[4096];
    struct sigaction act;
    act.sa_handler = f;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    // sigaction(SIGCHLD, &act, NULL); // zombie
    if(!(p = fork())) exit(1);
    sleep(1);
    snprintf(status, 256, "/proc/%d/status", p);
    if((f1 = open(status, O_RDONLY)) != -1)
        n = read(f1, buf, 4096), write(1, buf, n), close(f1);
    else
        perror(status);
    return 0;
}