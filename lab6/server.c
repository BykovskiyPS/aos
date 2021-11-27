#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <syslog.h>

#define MAX_NUMBER 100
#define NAME "/home/pavel/aos/lab6/.config"
#define MAXNAME 255

extern int errno;
char name[MAXNAME + 1];
char * message;
char* logfile;
int port, semid, shmid;
FILE* flog;

typedef struct {
    int numbers[MAX_NUMBER];
} message_t;

// для записи в shm
message_t* msg;

void print_shm(int numbers[], int count){
    for(int i = 0; i < count; i++){
        // printf("%d ", numbers[i]);
        fprintf(flog, "%d ", numbers[i]);
    }
    // printf("\n");
    fprintf(flog, "\n");
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

void test(int numbers[], int count){
    // Before modified
    // printf("~~~Test\n\tRaw data: ");
    fprintf(flog, "~~~Test\n\tRaw data: ");
    for(int i = 0; i < count; i++){
        // printf("%d ", numbers[i]);
        fprintf(flog, "%d ", numbers[i]);
    }
    // printf("\n\tAfter modified: ");
    fprintf(flog, "\n\tAfter modified: ");

    // Modify data
    for(int i = 0; i < count; i++){
        int tmp = numbers[i] - 1;
        numbers[i] <<= 1;
        numbers[i] += tmp;
        numbers[i] <<= 1;
    }

    // After modified
    print_shm(numbers, count);
}

void calculate(int* numbers, int n){
    /*  
        ЛОГИКА ВЗАИМОДЕЙСТВИЯ ПРОЦЕССОВ:
        P2: сдвигает влево на 1 число и отправляет P1
        P1: вычитает 1 из сообщения от сервера и + с числом от P2
        P3: сдвигает влево на 1 число от P1 и передает серверу
    */

    // test
    int copy_msg[n];
    memcpy(copy_msg, msg->numbers, n*sizeof(int));
    fprintf(flog, "%d\n", getpid());
    test(copy_msg, n);

    struct sembuf s;
    int f_chld;
    char buf[10];
    int fd[2];
    pipe(fd);
    for(int i = 0; i < 3; i++){
        if(!fork()){
            if(i == 0){
                close(fd[0]);
                f_chld = getpid();
                sprintf(buf, "%d", f_chld);
                for(int i = 0; i < 2; i++)
                    write(fd[1], buf, 10);
                // close(fd[1]);
            } else {
                close(fd[1]);
                read(fd[0], buf, 10);
                f_chld = atoi(buf);
                // close(fd[0]);
            }
            switch (getpid() - f_chld){
                case 0: ;
                    // printf("===P1:coping data from shm\n");
                    // fprintf(flog, "===P1:coping data from shm\n");
                    int copy[MAX_NUMBER];
                    memcpy(copy, numbers, n*sizeof(int));
                    // передаем управление 2му процессу, увеличивая значение 2го семафора на 1
                    s.sem_num = 2;
                    s.sem_op = 1;
                    s.sem_flg = 0;
                    if(semop(semid, &s, 1) == -1){
                        // perror("Error 1 in process 1");
                        fprintf(flog, "Error 1 in process 1 %s\n", sys_errlist[errno]);
                        exit(1);
                    }

                    // ждем своей очереди от 2го процесса, т.е. пока он не увеличит значение 1го семафора на 1
                    s.sem_num = 1;
                    s.sem_op = -1;
                    s.sem_flg = 0;
                    if(semop(semid, &s, 1) == -1){
                        // perror("Error 2 in process 1");
                        fprintf(flog, "Error 2 in process 1 %s\n", sys_errlist[errno]);
                        exit(1);
                    }
                    // printf("===P1:modify data\n");
                    // fprintf(flog, "===P1:modify data\n");
                    for(int i = 0; i < n; i++){
                        int tmp = copy[i] - 1;
                        usleep(rand() % 100);
                        numbers[i] += tmp;
                        usleep(rand() % 100);
                    }

                    // передаем управление 3му процессу, увеличивая значение 3го семафора на 1, завершаем работу
                    s.sem_num = 3;
                    s.sem_op = 1;
                    s.sem_flg = 0;
                    if(semop(semid, &s, 1) == -1){
                        // perror("Error 2 in process 1");
                        fprintf(flog, "Error 2 in process 1 %s\n", sys_errlist[errno]);
                        exit(1);
                    }
                    // printf("===P1:Exit\n");
                    // fprintf(flog, "===P1:Exit\n");
                    exit(0);
                case 1:
                    // ждем своей очереди от 1го процесса, т.е. пока он не увеличит 2й семафор на 1
                    s.sem_num = 2;
                    s.sem_op = -1;
                    s.sem_flg = 0;
                    if(semop(semid, &s, 1) == -1){
                        // perror("Error 1 in process 2");
                        fprintf(flog, "Error 1 in process 2 %s\n", sys_errlist[errno]);
                        exit(1);
                    }
                    // printf("===P2:modify data\n");
                    // fprintf(flog, "===P2:modify data\n");
                    for(int i = 0; i < n; i++){
                        numbers[i] <<= 1;
                        usleep(rand() % 100);
                    }

                    // передаем управление 1му процессу, увеличивая 1й семафор на 1, и завершаем процесс
                    s.sem_num = 1;
                    s.sem_op = 1;
                    s.sem_flg = 0;
                    if(semop(semid, &s, 1) == -1){
                        // perror("Error 2 in process 2");
                        fprintf(flog, "Error 2 in process 2 %s\n", sys_errlist[errno]);
                        exit(1);
                    }
                    // printf("===P2:Exit\n");
                    // fprintf(flog, "===P2:Exit\n");
                    exit(0);
                case 2:
                    // ждем своей очереди от 1го процесса, т.е. пока он не увеличит 3й семафор на 1, завершаем работу
                    s.sem_num = 3;
                    s.sem_op = -1;
                    s.sem_flg = 0;
                    if(semop(semid, &s, 1) == -1){
                        // perror("Error 1 in process 3");
                        fprintf(flog, "Error 1 in process 3 %s\n", sys_errlist[errno]);
                        exit(1);
                    }
                    // printf("===P3:modify data\n");
                    // fprintf(flog, "===P3:modify data\n");
                    for(int i = 0; i < n; i++){
                        numbers[i] <<= 1;
                        usleep(rand() % 100);
                    }
                    // printf("===P3:Exit\n");
                    // fprintf(flog, "===P3:Exit\n");
                    exit(0);
                default:
                    // printf("Process=%d Parent=%d FirstChld=%d\n", getpid(), getppid(), f_chld);
                    fprintf(flog, "Process=%d Parent=%d FirstChld=%d\n", getpid(), getppid(), f_chld);
                    // perror("Bad PID");
                    fprintf(flog, "Bad PID %s\n", sys_errlist[errno]);
                    exit(1);
            }
        }
    }
    // ожидаем завершение всех дочерних процессов
    int pid;
    while((pid = wait(0)) > 0);
}

int read_message(char* string){
    char* f = strchr(string, ' ');
    char buf[10];
    strncpy(buf, string, f - string);
    // buf[f - string] = '\0';
    port = atoi(buf);
    logfile = f + 1;
}

void create_ipcs(){
    // генерация ключа
    key_t key;
    if((key = ftok(logfile, 'A')) == -1){
        perror("Generate key");
        exit(1);
    }

    // создание разделяемой памяти
    if((shmid = shmget(key, sizeof(message_t), 0600 | IPC_CREAT | IPC_EXCL)) < 0){
        perror("Create shared memory");
        exit(1);
    }

    // подключение сегмента разделяемой памяти к процессу
    if((msg = (message_t*) shmat(shmid, NULL, SHM_RND)) == NULL){
        perror("Attach shared memory");
        exit(1);
    }

    // создание множества семафоров
    if((semid = semget(key, 4, 0666 | IPC_CREAT | IPC_EXCL)) == -1){
        perror("Create semmaphores");
        exit(1);
    }
}

void delete_ipcs(){
    // удаляем разделяемую память
    if(shmctl(shmid, 0, IPC_RMID) == -1){
        perror("Delete shm");
        exit(1);
    }
    // удаляем множество семафоров
    if(semctl(semid, 0, IPC_RMID) == -1){
        perror("Delete semaphores");
        exit(1);
    }
}

void mydaemon() {
    // становимся лидером сеанса
    setsid();

    // открываем файл логов, указанный в конфигурации
    if((flog = fopen(logfile, "w+")) == NULL){
        perror(logfile);
        fprintf(flog, "flog %s", sys_errlist[errno]);
        exit(1);
    }
    setvbuf(flog, NULL, _IOLBF, 0);

    // openlog("message", LOG_PID | LOG_CONS, LOG_USER);
    // syslog(LOG_INFO, "%s", "Started");
    printf("===Started\n");
    fprintf(flog, "===Started\n");
    // пишем то, что скопировали из конфигурационного файла
    // syslog(LOG_INFO, "%s", message);
    fprintf(flog, "===%s\n", message);
    create_ipcs();
    // настройка сервера
    char buf[4096];
    int s, news;
    struct sockaddr_in sa;

    if((s = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        // perror("socket");
        fprintf(flog, "Socket %s", sys_errlist[errno]);
        exit(1);
    }
    memset(&sa, 0, sizeof(sa));
    sa.sin_family = AF_INET;
    sa.sin_port = htons(port);
    if(bind(s, (const struct sockaddr *)&sa, sizeof(sa)) == -1){
        // perror("bind");
        fprintf(flog, "Bind %s", sys_errlist[errno]);
        exit(1);
    }
    if(listen(s, 5) == -1){
        // perror("listen");
        fprintf(flog, "Listen %s", sys_errlist[errno]);
        exit(1);
    }

    // чтобы наши процессы, которые мы будем создавать не оставались зомби
    signal(SIGCHLD, SIG_IGN);
    for(;;){
        // возвращает новый сокет, который связан с текущим клиентом
        // а текущий сокет можно опять вызывать accept, чтобы получать новые соединения
        if((news = accept(s, NULL, NULL)) == -1){
            // perror("accept");
            fprintf(flog, "Accept %s", sys_errlist[errno]);
            exit(1);
        }
        if(fork()){
            // закрываем сокет, он нам больше не нужен
            close(news);
        } else {
            // дочерний процесс обрабатывает текущее соединение
            // сокет не нужен через который мы ждем входящее соединение
            close(s);
            read(news, buf, 4096);
            int n = amount_numbs(buf) + 1;
            int numbers[n];
            split(numbers, buf, n);
            memset(buf, 0, sizeof(buf));

            // копируем в разделяему память полученные данные
            for(int i = 0; i < n; i++){
                msg->numbers[i] = numbers[i];
            }

            // test
            // int copy_msg[n];
            // memcpy(copy_msg, msg->numbers, n*sizeof(int));
            // fprintf(flog, "%d\n", getpid());
            // test(copy_msg, n);

            // основная логика
            calculate(msg->numbers, n);

            // printf("===Result: ");
            fprintf(flog, "===Result: ");
            print_shm(msg->numbers, n);

            concat_nums(buf, msg->numbers, n);
            write(news, buf, 4096);
            // закроем сокет
            shutdown(0, SHUT_RDWR);
            exit(0);
        }
    }
}

void term() {
    // Завершение. Если пришел сигнал term
    // syslog(LOG_INFO, "%s", "Finished");
    // printf("===%s", "Finished");
    fprintf(flog, "===Finished");
    free(message);
    delete_ipcs();
    fclose(flog);
    // closelog();
    exit(0);
}

void hup() {
    // Перечитать свой файл конфигурации
    int f;
    struct stat st;
    free(message);
    message = NULL;
    if((f = open(name, O_RDONLY)) == -1){
        // syslog(LOG_INFO, "%s: %s", name, strerror(errno));
        // perror("Name");
        fprintf(flog, "Name %s", sys_errlist[errno]);
        term();
    }
    if(fstat(f, &st) == -1){
        // syslog(LOG_INFO, "%s: %s", name, strerror(errno));
        // perror("Name");
        fprintf(flog, "Name %s", sys_errlist[errno]);
        term();
    }
    if((message = malloc(st.st_size + 1)) == NULL){
        // syslog(LOG_INFO, "%s: %s", "malloc", strerror(errno));
        // perror("Malloc");
        fprintf(flog, "Malloc %s", sys_errlist[errno]);
        term();
    }
    read(f, message, st.st_size);
    message[st.st_size - 1] = '\0';
    close(f);

    read_message(message);

    delete_ipcs();
    if((flog = fopen(logfile, "w+")) == NULL){
        // perror(logfile);
        fprintf(flog, "flog %s", sys_errlist[errno]);
        exit(1);
    }
    create_ipcs();

    // syslog(LOG_INFO, "%s", "Reconfigured");
    // syslog(LOG_INFO, "%s", message);
    fprintf(flog, "===Reconfigured\n");
    fprintf(flog, "===%s\n", message);
    // printf("===%s", message);
}

int main(int argc, char* argv[], char* envp[]){
    struct stat st;
    struct sigaction act;
    int f;
    if(argc != 1){
        fprintf(stderr, "Usage: %s\n", argv[0]);
        exit(1);
    }
    strcpy(name, NAME);
    if((f = open(name, O_RDONLY)) == -1){
        perror(name);
        exit(1);
    }
    if(fstat(f, &st) == -1){
        perror(name);
        exit(1);
    }
    if((message = malloc(st.st_size + 1)) == NULL){
        perror("Malloc");
        exit(1);
    }
    read(f, message, st.st_size);
    message[st.st_size - 1] = '\0';
    close(f);

    read_message(message);
    printf("%d\n%s\n", port, logfile);

    sigfillset(&act.sa_mask);
    act.sa_flags = SA_RESTART;
    act.sa_handler = hup;
    sigaction(SIGHUP, &act, NULL);

    act.sa_handler = term;
    sigaction(SIGTERM, &act, NULL);

    // закрываем все файлы
    for(f = 0; f < 256; f++)
        close(f);
    // меняем на корневой, чтобы демон не мешал размонитрованию ФС
    chdir("/");

    // порождаем дочерний процесс, который и будет демоном
    f = fork();
    switch(f) {
        case -1: 
            perror("fork");
            exit(1);
        case 0:
            mydaemon();
    }
    exit(0);
}