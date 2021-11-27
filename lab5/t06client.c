#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <malloc.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

struct message {
    long type;
    char text[1];
};

int main(int argc, char* argv[]){
    key_t key;
    int msgid, k, srvid;
    struct message* m;
    char* p;
    if(argc != 2){
        fprintf(stderr, "Not enought arguments %s", argv[0]);
        exit(1);
    }
    srvid = strtol(argv[1], &p, 10);

    // создаем очередь сообщений
    if((msgid = msgget(IPC_PRIVATE, IPC_CREAT | 0600)) == -1){
        perror("msgget");
        exit(1);
    }

    if((m = malloc(1024)) == NULL){
        fprintf(stderr, "Memory allocation error\n");
        exit(1);
    }

    m->type = 1;
    
    char* c;
    c = malloc(10 * sizeof(char));
    int v = 0;
    int n = msgid;
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
        // printf("Client before: %s\n", m->text);
        if(msgsnd(srvid, m, 1020, 0) == -1){
            perror("msgsnd");
            exit(1);
        }

        if((k = msgrcv(msgid, m, 1020, 1, 0)) == -1){
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