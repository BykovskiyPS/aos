#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

void concat_params(char* dst, char** argv, int argc){
    for(int i = 1; i < argc; i++){
        if(i != 1)
            strcat(dst, " ");
        strcat(dst, argv[i]);
    }
}

void split(int* dst, char* src, int count){
    char* istr;
    istr = strtok(src, " ");
    int cur = 0;
    while(istr != NULL){
        int n = atoi(istr);
        dst[cur++] = n;
        istr = strtok(NULL, " ");
    }
}

void concat_nums(char* dst, int* numbs, int count){
    char p[20];
    for(int i = 0; i < count; i++){
        if(i != 0) 
            strcat(dst, " ");
        sprintf(p, "%d", numbs[i]);
        strcat(dst, p);
    }
}

int amount_numbs(char* string){
    char s = ' ';
    int count = 0;
    for(int i = 0; i < strlen(string) - 1; i++){
        if(string[i] == s)
            count++;
    }
    return count;
}

int port;
char* file;

int read_message(char* string){
    char* f = strchr(string, ' ');
    char buf[10];
    strncpy(buf, string, f - string);
    buf[f - string] = '\0';
    port = atoi(buf);
    file = f + 1;
}

int main(int argc, char* argv[]){
    int f;
    if((f = open("syslog", O_WRONLY)) == -1){
        perror("syslog");
        exit(1);
    }
    close(1);
    dup(f);
    close(f);

    char buf[4096];
    concat_params(buf, argv, argc);
    printf("%s\n", buf);

    int numbers[argc - 1];
    split(numbers, buf, argc - 1);
    
    for(int i = 0; i < argc - 1; i++)
        printf("%d\n", numbers[i]);

    memset(buf, 0, sizeof(buf));
    printf("%s\n", buf);

    char numbs[4096];
    concat_nums(numbs, numbers, argc - 1);
    printf("%s\n", numbs);

    int n = amount_numbs(numbs) + 1;
    printf("%d\n", n);
    char str[] = "50001 mydaemonlord\0";
    read_message(str);
    printf("%d %s\n", port, file);
    return 0;
}