#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

extern int errno;
extern char ** environ;

// добавляем новые строки в среду процесса
// envp - копия окружения на момент старта программы
// environ - глобальная переменная окружения

int main(int argc, char* argv[], char* envp[]){
    putenv("A=123");
    putenv("B=abc");
    for(int i = 0; envp[i]; i++){
        printf("%s\n",envp[i]);
    }
    printf("===============Global environ===============\n");
    for(int i = 0; environ[i]; i++){
        printf("%s\n",environ[i]);
    }
    return 0;
}