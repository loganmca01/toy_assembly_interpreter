#include "vm.h"
#include <stdlib.h>
#include <sys/msg.h>
#include <string.h>

int msqid;


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

int receive_message(struct in_msgbuf *in) {

    key_t key = ftok("..", 'A');

    if (key == -1) {
        perror("generating system v key");
        return -1;
    }

    if (key == -1) {
        perror("ftok");
        return 1;
    }

    int msqid = msgget(key, 0666);

    if (msqid == -1) {
        perror("msgget");
        return 1;
    }

    if (msgrcv(msqid, in, sizeof in->mtext, 2, 0) == -1) {
        perror("msgrcv");
        return 1;
    }

    return 0;

}