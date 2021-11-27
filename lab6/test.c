#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

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

int main(int argc, char* argv[]) {
    char buf[4096];
    int s, news;
    struct sockaddr_in sa;

    if((s = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        perror("socket");
        exit(1);
    }
    memset(&sa, 0, sizeof(sa));
    sa.sin_family = AF_INET;
    sa.sin_port = htons(5000);
    if(bind(s, (const struct sockaddr *)&sa, sizeof(sa)) == -1){
        perror("bind");
        exit(1);
    }
    if(listen(s, 5) == -1){
        perror("listen");
        exit(1);
    }
    // чтобы наши процессы, которые мы будем создавать не оставались зомби
    signal(SIGCHLD, SIG_IGN);
    for(;;){
        // возвращает новый сокет, который связан с текущим клиентом
        // а текущий сокет можно опять вызывать accept, чтобы получать новые соединения
        if((news = accept(s, NULL, NULL)) == -1){
            perror("accept");
            exit(1);
        }
        if(fork())
            // закрываем сокет, он нам больше не нужен
            close(news);
        else {
            // дочерний процесс обрабатывает текущее соединение
            // сокет не нужен через который мы ждем входящее соединение
            close(s);
            // for(int i = 0; i < 2; i++)
            //     close(i);
            // for(int i = 0; i < 2; i++)
            //     dup(news);
            // // отмена буферизации в стд. потоке
            // setbuf(stdin, NULL);
            // setbuf(stdout, NULL);
            // setbuf(stderr, NULL);
            read(news, buf, 4096);
            int n = amount_numbs(buf) + 1;
            int numbers[n];
            split(numbers, buf, n);
            memset(buf, 0, sizeof(buf));

            // основная логика работы с данными
            for(int i = 0; i < n; i++){
                numbers[i]++;
            }

            concat_nums(buf, numbers, n);
            write(news, buf, 4096);
            // закроем сокет
            shutdown(0, SHUT_RDWR);
            exit(0);
        }
    }
    return 0;
}