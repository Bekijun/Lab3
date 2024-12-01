#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

int custom_system(const char *command) {
    if (command == NULL) {
        fprintf(stderr, "Command cannot be NULL\n");
        return -1;
    }

    pid_t pid = fork(); // 자식 프로세스 생성
    if (pid < 0) {
        perror("fork");
        return -1;
    } else if (pid == 0) {
        // 자식 프로세스에서 명령 실행
        char *args[50]; // 최대 50개의 인자를 처리
        char *cmd = strdup(command); // 원본 명령어 복사
        int i = 0;

        // 명령어와 인자 파싱
        args[i] = strtok(cmd, " ");
        while (args[i] != NULL && i < 49) {
            args[++i] = strtok(NULL, " ");
        }
        args[i] = NULL;

        // 명령 실행
        execvp(args[0], args); // 명령 실행
        perror("execvp"); // 실패 시 에러 출력
        free(cmd);
        exit(1); // 자식 프로세스 종료
    } else {
        // 부모 프로세스는 자식의 종료를 기다림
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            return WEXITSTATUS(status); // 자식의 종료 코드 반환
        } else {
            return -1; // 비정상 종료
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <command>\n", argv[0]);
        return 1;
    }

    // 사용자 입력으로부터 명령어 생성
    char command[256] = {0};
    for (int i = 1; i < argc; i++) {
        strcat(command, argv[i]);
        if (i < argc - 1) strcat(command, " "); // 인자 사이에 공백 추가
    }

    printf("Executing command: %s\n", command);

    // 직접 구현한 custom_system 호출
    int result = custom_system(command);

    // 결과 출력
    if (result == 0) {
        printf("Command executed successfully.\n");
    } else {
        printf("Command execution failed with code: %d\n", result);
    }

    return 0;
}
