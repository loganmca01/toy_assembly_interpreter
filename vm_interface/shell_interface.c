#include <stdlib.h>
#include <stdio.h>
#include <sys/msg.h>

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

int main(void) {

    struct out_msgbuf out;

    key_t key = ftok("..", 'A');

    if (key == -1) {
        perror("ftok");
        return 1;
    }

    int msqid = msgget(key, 0666 | IPC_CREAT);

    if (msqid == -1) {
        perror("msgget");
        return 1;
    }

    for(;;) { 
        if (msgrcv(msqid, &out, sizeof out.mtext, 0, 0) == -1) {
            perror("msgrcv");
            msgctl(msqid, IPC_RMID, NULL);
            return 1;
        }

        

    }

    msgctl(msqid, IPC_RMID, NULL);


}