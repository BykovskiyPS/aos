#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

// принимаем сообщения из очереди и выводим их экран

struct message {
    long type;
    char text[1];
};

int main(int argc, char* argv[]) {
    struct message* m;
    char* p;
    int msgid, msgtype, k;
    if(argc != 3){
        perror("Invalid arguments");
        exit(1);
    }
    msgid = strtol(argv[1], &p, 10);
    if(argv[1] == p || *p){
        fprintf(stderr, "value msgid %s not valid\n", argv[1]);
        exit(1);
    }
    msgtype = strtol(argv[2], &p, 10);
    if(argv[2] == p || *p){
        fprintf(stderr, "value msgtype %s not valid\n", argv[2]);
        exit(1);
    }
    if((m = malloc(4096)) == NULL){
        fprintf(stderr, "Memory allocation error\n");
        exit(1);
    }

    // п.2
    // if((k = msgrcv(msgid, m, 4096, msgtype, 0)) == -1){
    //     perror("msgrcv");
    //     free(m);
    //     exit(1);
    // }

    // п.3
    // if((k = msgrcv(msgid, m, 4096, msgtype, IPC_NOWAIT)) == -1){
    //     perror("msgrcv");
    //     free(m);
    //     exit(1);
    // }

    // п.4 
    // if((k = msgrcv(msgid, m, 5, msgtype, MSG_NOERROR)) == -1){
    //     perror("msgrcv");
    //     free(m);
    //     exit(1);
    // }

    if((k = msgrcv(msgid, m, 5, msgtype, 0)) == -1){
        perror("msgrcv");
        free(m);
        exit(1);
    }

    printf("Type=%d\n", msgtype);
    printf("Message '%s'\n", m->text);
    free(m);
    return 0;
}