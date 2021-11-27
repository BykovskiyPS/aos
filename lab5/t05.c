#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

int main(int argc, char* argv[]) {
    int msgid;
    char* p;
    if(argc != 2){
        fprintf(stderr, "Not enought arguments %s", argv[0]);
        exit(1);
    }
    msgid = strtol(argv[1], &p, 10);
    if(argv[1] == p || *p){
        fprintf(stderr, "value msgid %s not valid\n", argv[1]);
        exit(1);
    }
    if(msgctl(msgid, IPC_RMID, 0) == -1){
        perror("msgctl");
        exit(1);
    };
    return 0;
}