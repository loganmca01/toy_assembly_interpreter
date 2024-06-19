#include "vm.h"
#include <stdlib.h>
#include <sys/msg.h>
#include <string.h>

int msqid;

#define IN_MAX 256
#define OUT_MAX 1024

struct in_msgbuf {
    long mtype;
    char mtext[IN_MAX];
};

struct out_msgbuf {
    long mtype;
    char mtext[OUT_MAX];
};

int open_connection() {

    key_t key = ftok("..", 'A');

    if (key == -1) {
        perror("generating system v key");
        return -1;
    }
    
    msqid = msgget(key, 0666);

    if (msqid == -1) {
        return 1;
    }

    printf("%d\n", msqid);

    return 0;

}

int send_message(char *data) {

    struct out_msgbuf out;
    int len;

    if ((len = strlen(data)) >= OUT_MAX) {
        fprintf(stderr, "message too large\n");
        return 1;
    }

    out.mtype = 1;
    
    strcpy(out.mtext, data);

    if (msgsnd(msqid, &out, OUT_MAX, 1) == -1) {
        perror("msgsnd");
        return 1;
    } 

    return 0;

}

char *receive_message() {
    
}