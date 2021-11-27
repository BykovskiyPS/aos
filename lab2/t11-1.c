#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>

extern int errno;

// Выполнить команду shell с помощью библиотечной ф-ции system

char* make_string(int argc, char* argv[]){
    // считаем длину всех аргументов + количество пробелов
    int len = 0;
    for(int i = 1; i < argc; i++){
        len += strlen(argv[i]);
    }
    len += argc - 2; // spaces

    // создаем эту строку в куче
    char* p_string = (char*) malloc(len);
    if(p_string == NULL) { exit(1); }

    // посимвольно записываем в строку символы из командной строки
    int k = 0;
    for(int i = 1; i < argc; i++){
        for(int j = 0; j < strlen(argv[i]); j++){
            p_string[k++] = argv[i][j];
        }
        if(i < argc - 1){
            p_string[k++] = ' ';
        } 
    }
    p_string[k++] = '\0';
    
    return p_string;
}

int main(int argc, char* argv[], char* envp[]){
    if(argc < 2){
        fprintf(stderr, "Bad number arguments %s\n", argv[0]);
        exit(1);
    }
    system(make_string(argc, argv));
    return 0;
}