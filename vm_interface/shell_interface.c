#include <stdlib.h>
#include <stdio.h>
#include <sys/msg.h>
#include <string.h>

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

    struct out_msgbuf rec;
    struct in_msgbuf send;

    send.mtype = 2;

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

    /* get connection succeeded message */
    if (msgrcv(msqid, &rec, sizeof rec.mtext, 0, 0) == -1) {
        perror("msgrcv");
        msgctl(msqid, IPC_RMID, NULL);
        return 1;
    }

    printf("%s", rec.mtext);

    while(fgets(send.mtext, sizeof send.mtext, stdin) != NULL) {
        
        int len = strlen(send.mtext);

        if (send.mtext[len-1] == '\n') send.mtext[len-1] = '\0';

        if (msgsnd(msqid, &send, len, 0) == -1) {
            perror("msgsnd");
            return 1;
        }

        if (!strcmp(send.mtext, "quit")) break;

    }

    msgctl(msqid, IPC_RMID, NULL);


}