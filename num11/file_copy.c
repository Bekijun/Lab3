#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>

#define SHM_SIZE 1024  // 공유 메모리 크기

// 세마포어 초기화 함수
void sem_init(int sem_id, int sem_num, int value) {
    if (semctl(sem_id, sem_num, SETVAL, value) == -1) {
        perror("semctl");
        exit(1);
    }
}

// 세마포어 잠금
void sem_wait(int sem_id, int sem_num) {
    struct sembuf op = {sem_num, -1, 0};
    if (semop(sem_id, &op, 1) == -1) {
        perror("semop wait");
        exit(1);
    }
}

// 세마포어 해제
void sem_signal(int sem_id, int sem_num) {
    struct sembuf op = {sem_num, 1, 0};
    if (semop(sem_id, &op, 1) == -1) {
        perror("semop signal");
        exit(1);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <source_file> <destination_file>\n", argv[0]);
        exit(1);
    }

    const char *source_file = argv[1];
    const char *destination_file = argv[2];

    // 공유 메모리 생성
    int shm_id = shmget(IPC_PRIVATE, SHM_SIZE, IPC_CREAT | 0666);
    if (shm_id == -1) {
        perror("shmget");
        exit(1);
    }

    // 세마포어 생성
    int sem_id = semget(IPC_PRIVATE, 2, IPC_CREAT | 0666);
    if (sem_id == -1) {
        perror("semget");
        exit(1);
    }

    // 세마포어 초기화
    sem_init(sem_id, 0, 1); // sem_empty
    sem_init(sem_id, 1, 0); // sem_full

    // 공유 메모리 연결
    char *shm_ptr = (char *)shmat(shm_id, NULL, 0);
    if (shm_ptr == (char *)-1) {
        perror("shmat");
        exit(1);
    }

    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        exit(1);
    }

    if (pid == 0) {
        // 자식 프로세스: 파일 쓰기
        FILE *dest = fopen(destination_file, "w");
        if (!dest) {
            perror("fopen destination");
            exit(1);
        }

        while (1) {
            sem_wait(sem_id, 1); // sem_full 대기

            // 공유 메모리에서 읽기
            if (strcmp(shm_ptr, "EOF") == 0) {
                break;
            }

            fprintf(dest, "%s", shm_ptr);

            sem_signal(sem_id, 0); // sem_empty 신호
        }

        fclose(dest);
        printf("파일 복사 완료: %s\n", destination_file);
        exit(0);
    } else {
        // 부모 프로세스: 파일 읽기
        FILE *src = fopen(source_file, "r");
        if (!src) {
            perror("fopen source");
            exit(1);
        }

        while (1) {
            sem_wait(sem_id, 0); // sem_empty 대기

            // 공유 메모리에 쓰기
            if (fgets(shm_ptr, SHM_SIZE, src) == NULL) {
                strcpy(shm_ptr, "EOF");
                sem_signal(sem_id, 1); // sem_full 신호
                break;
            }

            sem_signal(sem_id, 1); // sem_full 신호
        }

        fclose(src);

        // 자식 프로세스 종료 대기
        wait(NULL);

        // 공유 메모리 및 세마포어 삭제
        shmctl(shm_id, IPC_RMID, NULL);
        semctl(sem_id, 0, IPC_RMID);
        printf("공유 메모리와 세마포어 해제 완료\n");
    }

    return 0;
}
