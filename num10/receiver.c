#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdlib.h>

#define MSG_SIZE 100

struct msgbuf {
    long mtype;
    char mtext[MSG_SIZE];
};

int main() {
    key_t key;
    int msgid;
    struct msgbuf message;

    // 메시지 큐 키 생성
    key = ftok("chat", 65);
    if (key == -1) {
        perror("ftok");
        exit(1);
    }

    // 메시지 큐 생성/접근
    msgid = msgget(key, 0666 | IPC_CREAT);
    if (msgid == -1) {
        perror("msgget");
        exit(1);
    }

    printf("메시지 수신 프로그램 시작 (종료: Ctrl+C)\n");
    while (1) {
        // 메시지 수신
        if (msgrcv(msgid, &message, sizeof(message.mtext), 1, 0) == -1) {
            perror("msgrcv");
            exit(1);
        }

        printf("받은 메시지: %s\n", message.mtext);
    }

    return 0;
}
