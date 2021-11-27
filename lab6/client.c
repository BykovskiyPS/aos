#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

int main(int argc, char* argv[]){
    int s, news, l;
    char buf[4096];
    struct sockaddr_in sa;
    struct hostent* h;
    if(argc < 2){
        fprintf(stderr, "Usage: %s host [numbers]\n", argv[0]);
        exit(1);
    }
    if((s = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        perror("socket");
        exit(1);
    }
    // по имени хоста получаем IP адрес
    if((h = gethostbyname(argv[1])) == NULL){
        herror(argv[1]);
        exit(1);
    }
    memset(&sa, 0, sizeof(sa));
    sa.sin_family = AF_INET;
    sa.sin_port = htons(9999);
    // копируем адрес
    memcpy(&sa.sin_addr, h->h_addr, 4);
    if(connect(s, (const struct sockaddr *)&sa, sizeof(sa)) == -1){
        perror("connect");
        exit(1);
    }

    l = read(0, buf, 4096);
    write(s, buf, l);

    read(s, buf, 4096);
    printf("%s\n", buf);

    shutdown(s, SHUT_RDWR);
    exit(0);
    return 0;
}