#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

extern int errno;

// max size form input files

int main(int argc, char* argv[]){
    if(argc == 1){
        fprintf(stderr, "Not enougth arguments\n");
        exit(1);
    }
    struct stat st;
    int size = 0;
    char* file;
    for(int i = 1; i < argc; i++){
        if(stat(argv[i], &st) == -1){
            perror(argv[i]);
        } else {
            // S_IFREG & st.st_mode - только для файлов
            if(S_IFREG & st.st_mode && st.st_size > size){
                size = st.st_size;
                file = argv[i];
            }
        }
    }
    printf("File %s with size=%d\n", file, size);
    return 0;
}