#include <stdlib.h>
#include <stdio.h>
#include <sys/msg.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

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

int msqid;

void sigint_handler(int sig);
void sigtstp_handler(int sig);
void sighup_handler(int sig);

/* See function definitions for sources of these */
typedef void handler_t(int);
handler_t *Signal(int signum, handler_t *handler);
void unix_error(char *msg);

int main(void) {

    struct out_msgbuf rec;
    struct in_msgbuf send;

    Signal(SIGINT, sigint_handler);
    Signal(SIGTSTP, sigtstp_handler);
    Signal(SIGHUP, sighup_handler);

    send.mtype = 2;

    key_t key = ftok("..", 'A');

    if (key == -1) {
        perror("ftok");
        return 1;
    }

    msqid = msgget(key, 0666 | IPC_CREAT);

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

    printf("> ");

    while(fgets(send.mtext, sizeof send.mtext, stdin) != NULL) {
        
        int len = strlen(send.mtext);

        if (send.mtext[len-1] == '\n') send.mtext[len-1] = '\0';

        if (msgsnd(msqid, &send, len, 2) == -1) {
            perror("msgsnd");
            return 1;
        }

        if (!strcmp(send.mtext, "quit")) break;

        if (msgrcv(msqid, &rec, sizeof rec.mtext, 1, 0) == -1) {
            perror("msgrcv");
            msgctl(msqid, IPC_RMID, NULL);
            return 1;
        }

        printf("%s\n", rec.mtext);

        printf("> ");

    }

    msgctl(msqid, IPC_RMID, NULL);


}

/*
* Signal wrapper credited to W. R. Stevens, B. Fenner, and A. M. Rudoff's 'Unix Network Programming: The Sockets Networking API, Third Edition' 
* found by me in Bryant & O'Hallaron Computer Systems: A Programmer's Perspective
*/
handler_t *Signal(int signum, handler_t *handler) 
{
    struct sigaction action, old_action;

    action.sa_handler = handler;  
    sigemptyset(&action.sa_mask); /* block sigs of type being handled */
    action.sa_flags = SA_RESTART; /* restart syscalls if possible */

    if (sigaction(signum, &action, &old_action) < 0)
	unix_error("Signal error");
    return (old_action.sa_handler);
}

/*
 * standard Unix error reporting function, found in Bryant & O'Hallaron Computer Systems: A Programmer's Perspective
 */
void unix_error(char *msg)
{
    fprintf(stderr, "%s: %s\n", msg, strerror(errno));
    exit(1);
}

void sigint_handler(int sig) {

    if (msgctl(msqid, IPC_RMID, NULL) != 0) 
        unix_error("msgctl rmid");

    if (kill(getpid(), SIGKILL))
        unix_error("kill");

    return;

}

void sigtstp_handler(int sig) {

    if (msgctl(msqid, IPC_RMID, NULL) != 0) 
        unix_error("msgctl rmid");

    if (kill(getpid(), SIGKILL))
        unix_error("kill");

    return;

}

void sighup_handler(int sig) {

    if (msgctl(msqid, IPC_RMID, NULL) != 0) 
        unix_error("msgctl rmid");

    if (kill(getpid(), SIGKILL))
        unix_error("kill");

    return;

}