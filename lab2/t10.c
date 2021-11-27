#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>

extern int errno;

int main(int argc, char* argv[], char* envp[]){
    if(argc < 2){
        fprintf(stderr, "Bad number arguments %s\n", argv[0]);
        exit(1);
    }
    int rc;
    if(fork()){
        printf("Parent Arguments: ");
        for(int i = 0; i < argc; i++){
            printf("%s, ", argv[i]);
        }
        printf("\nParent enviroment\n");
        for(int i = 0; envp[i]; i++){
            printf("%s\n", envp[i]);
        }
        wait(&rc);
        exit(0);
    } else {
        printf("Child Arguments: ");
        for(int i = 0; i < argc; i++){
            printf("%s, ", argv[i]);
        }
        printf("\nChild enviroment\n");
        for(int i = 0; envp[i]; i++){
            printf("%s\n", envp[i]);
        }
        execvp(argv[1], argv+1);
        perror(argv[1]);
        exit(1);
    }
    return 0;
}