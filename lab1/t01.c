#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

// Проверка ошибок с помощью: errno, sys_errllist[], perror

extern int errno;

int main(int argsc, char* argv[]){
    errno = 0;
    int fd = open("file.txt", O_RDONLY);
    if(fd == -1){
        perror("Error");
        printf("Error from strerror: %s\n", sys_errlist[errno]);
        printf("Error: %d\n", errno);    
    }
    return 0;
}