#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <malloc.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <signal.h>

struct message {
    long type;
    char text[1];
};

int main(int argc, char* argv[]){
    int k, msgid;
    struct message* m;
    char* p;
    if(argc != 2){
        fprintf(stderr, "Not enought arguments %s", argv[0]);
        exit(1);
    }
    msgid = strtol(argv[1], &p, 10);

    if((m = malloc(1024)) == NULL){
        fprintf(stderr, "Memory allocation error\n");
        exit(1);
    }

    // сообщение для сервера
    m->type = 1;
    
    // переводим число в строку
    char* c;
    c = malloc(10 * sizeof(char));
    int v = 0;
    int n = getpid();
    while(n > 9){
        c[v++] = (n % 10) + '0';
        n = n / 10;
    }
    c[v++] = n + '0';
    c[v] = '\0';
    char t;
    //инвертируем массив символов
    for (int i = 0; i < v / 2; i++)
    {
        t = c[i];
        c[i] = c[v - 1 - i];
        c[v - 1 - i] = t;
    }

    int l;
    char buf[4096];
    while((l = read(0, buf, 1024)) > 0){
        strcat(m->text, c);
        strcat(m->text, "/");
        strcat(m->text, buf);
        
        if(msgsnd(msgid, m, 1020, 0) == -1){
            perror("msgsnd");
            exit(1);
        }

        if((k = msgrcv(msgid, m, 1020, getpid(), 0)) == -1){
            perror("msgrcv");
            free(m);
            exit(1);
        }

        printf("Client received: %s\n", m->text);
        m->text[0] = 0;
    }

    free(m);
    return 0;
}