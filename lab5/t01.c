#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <pwd.h>
#include <grp.h>

// создаем очередь, пишем в нее сообщения и выводим информацию о ней

struct message {
    long type;
    char text[1];
};

int main(int argc, char* argv[]) {
    key_t key;
    int msgid, k;
    struct message* m1;

    // переменные для статистики
    struct msqid_ds m;
    struct passwd* p;
    struct group* g;
    char* pp;

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

    // пишем сообщения
    m1 = malloc(14);
    for(int i = 1; i <= 5; i++){
        m1->type = i;
        strcpy(m1->text, "123456789");
        if(msgsnd(msgid, m1, 9, 0) == -1){
            perror("msgsnd");
            exit(1);
        }
    }
    free(m1);

    // получаем информацию об очереди сообщений
    if(msgctl(msgid, IPC_STAT, &m) == -1){
        perror("msgctl");
        exit(1);
    }
    printf("Key 0x%x\n", m.msg_perm.__key);
    p = getpwuid(m.msg_perm.uid);
    printf("Owner %s\n", p->pw_name);
    g = getgrgid(m.msg_perm.gid);
    printf("Group %s\n", g->gr_name);
    p = getpwuid(m.msg_perm.cuid);
    printf("Creator %s\n", p->pw_name);
    g = getgrgid(m.msg_perm.cgid);
    printf("Creator group %s\n", g->gr_name);
    printf("Permission 0%o\n", m.msg_perm.mode);
    printf("Time of last msgsnd %s", ctime(&m.msg_stime));
    printf("Time of last msgrcv %s", ctime(&m.msg_rtime));
    printf("Time of last change %s", ctime(&m.msg_ctime));
    printf("Current number of bytes in queue %lu\n", m.__msg_cbytes);
    printf("Current number of message in queue %lu\n", m.msg_qnum);
    printf("Maximum number of bytes allowed in queue %lu\n", m.msg_qbytes);
    printf("PID of last msgsnd %u\n", m.msg_lspid);
    printf("PID of last msgrcv %u\n", m.msg_lrpid);
    return 0;
}