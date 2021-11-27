#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

extern int errno;

// очередность выполнения после fork()

int main(int argc, char* argv[]){
    close(creat("file", 0600));
    int l = fork();
    if(unlink("file") != -1){
        // # - parent
        // * - child
        write(1, l ? "#" : "*", 1);
    }
    return 0;
}