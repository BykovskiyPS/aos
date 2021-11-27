#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <malloc.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <signal.h>

struct message {
    long type;
    char text[1];
};

int main(int argc, char* argv[]){
    key_t key;
    int k, msgid;
    // struct message* m;
    char* p;

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

    // if((m = malloc(1024)) == NULL){
    //     fprintf(stderr, "Memory allocation error\n");
    //     exit(1);
    // }

    if(fork()){
        struct message* m;
        m = malloc(10);
        if(msgrcv(msgid, m, 10, 2, 0) == -1){
            perror("rcv");
            exit(1);
        }
        printf("I receive message with type=%ld: %s!\n",m->type, m->text);
        free(m);
        wait(NULL);
        exit(0);
    } else {
        struct message* m1;
        m1 = malloc(10);
        m1->type = 2;
        strcpy(m1->text, "Child");
        sleep(2);
        if(msgsnd(msgid, m1, 10, 0) == -1){
            perror("snd");
            exit(1);
        }
        free(m1);
        sleep(2);
    }

    return 0;
}