#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>

extern int errno;

int main(int argc, char* argv[]){
    if(argc != 2){
        fprintf(stderr, "Not enougth arguments\n");
        exit(1);
    }
    struct stat st;
    if(stat(argv[1], &st) == -1){
        perror("Stat");
        exit(1);
    }
    printf("Device=%ld Inode=%ld Mode=%o\n", st.st_dev, st.st_ino, st.st_mode);
    printf("Links=%ld UID=%d GID=%o\n", st.st_nlink, st.st_uid, st.st_gid);
    printf("Size o block=%ld, Count of blocks=%ld\n", st.st_blksize, st.st_blocks);
    printf("atime=%s", ctime(&st.st_atime));
    printf("mtime=%s", ctime(&st.st_mtime));
    printf("ctime=%s", ctime(&st.st_ctime));
    return 0;
}