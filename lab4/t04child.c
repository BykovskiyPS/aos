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
    char buf[80];
    int l;
    while((l = read(0, buf, 80)) > 0){
        write(1, buf, l);
    }
    exit(0);
}
