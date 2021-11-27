#include <stdio.h>
#include <string.h>
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
    int msgid, k;
    struct message* m;

    // получаем ключ
    if((key = ftok(argv[0], 'A')) == -1){
        perror(argv[0]);
        exit(1);
    }

    // создаем очередь сообщений
    if((msgid = msgget(key, IPC_CREAT | 0600)) == -1){
        perror("msgget");
        exit(1);
    }

    if((m = malloc(1024)) == NULL){
        fprintf(stderr, "Memory allocation error\n");
        exit(1);
    }
    
    while(1){
        // принимает сообщения из своей очереди
        if((k = msgrcv(msgid, m, 1020, 0, 0)) == -1){
            perror("msgrcv");
            free(m);
            exit(1);
        }

        // printf("I rcv msg\n");
        // printf("k=%d\n", k);
        // printf("Server received: %s\n", m->text);

        char* s;
        s = strchr(m->text, '/');
        char digit[s - m->text];
        strncpy(digit, m->text, s - m->text);
        int number = atoi(digit);
        char message[s - m->text];
        strcpy(message, s + 1);
        char srv_msg[1020] = "Server received ";
        strcat(srv_msg, message);

        printf("Server received: %s\n", m->text);
        // пишем сообщение в очередь клиента
        m->type = 1;
        strcpy(m->text, srv_msg);
        if(msgsnd(number, m, 1020, 0) == -1){
            perror("msgsnd");
            exit(1);
        }

        m->text[0] = 0;
    }
    
    free(m);
    return 0;
}