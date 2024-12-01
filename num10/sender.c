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

    printf("메시지 전송 프로그램 시작 (종료: exit 입력)\n");
    while (1) {
        // 사용자 입력
        printf("보낼 메시지: ");
        fgets(message.mtext, MSG_SIZE, stdin);
        message.mtext[strcspn(message.mtext, "\n")] = '\0'; // 개행 제거

        // 종료 조건
        if (strcmp(message.mtext, "exit") == 0) {
            break;
        }

        // 메시지 타입 설정 및 전송
        message.mtype = 1;
        if (msgsnd(msgid, &message, sizeof(message.mtext), 0) == -1) {
            perror("msgsnd");
            exit(1);
        }
    }

    printf("메시지 전송 종료\n");
    return 0;
}
