#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_SENTENCE_LENGTH 256

// 문장 파일 읽기
int load_sentences(const char *filename, char sentences[][MAX_SENTENCE_LENGTH], int max_sentences) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("파일을 열 수 없습니다");
        return -1;
    }

    int count = 0;
    while (fgets(sentences[count], MAX_SENTENCE_LENGTH, file) && count < max_sentences) {
        // 문장 끝의 개행 문자 제거
        sentences[count][strcspn(sentences[count], "\n")] = '\0';
        count++;
    }
    fclose(file);
    return count;
}

int main() {
    char sentences[10][MAX_SENTENCE_LENGTH]; // 최대 10개의 문장
    int sentence_count = load_sentences("sentences.txt", sentences, 10);
    if (sentence_count <= 0) {
        printf("문장을 로드할 수 없습니다. 프로그램을 종료합니다.\n");
        return 1;
    }

    printf("타자 연습 프로그램에 오신 것을 환영합니다!\n");
    printf("문장을 정확히 입력하세요. 시작하려면 Enter를 누르세요.\n");
    getchar(); // Enter 대기

    int total_errors = 0;
    int total_words = 0;
    double total_time = 0.0;

    for (int i = 0; i < sentence_count; i++) {
        printf("\n문장: %s\n", sentences[i]);

        char user_input[MAX_SENTENCE_LENGTH];
        printf("입력: ");
        
        // 입력 시작 시간 기록
        struct timespec start_time, end_time;
        clock_gettime(CLOCK_MONOTONIC, &start_time);

        fgets(user_input, MAX_SENTENCE_LENGTH, stdin);

        // 입력 종료 시간 기록
        clock_gettime(CLOCK_MONOTONIC, &end_time);

        // 입력 끝의 개행 문자 제거
        user_input[strcspn(user_input, "\n")] = '\0';

        // 시간 계산 (초 단위)
        double elapsed_time = (end_time.tv_sec - start_time.tv_sec) +
                              (end_time.tv_nsec - start_time.tv_nsec) / 1e9;
        total_time += elapsed_time;

        // 오류 계산
        int errors = 0;
        int length = strlen(sentences[i]) > strlen(user_input) ? strlen(sentences[i]) : strlen(user_input);
        for (int j = 0; j < length; j++) {
            if (sentences[i][j] != user_input[j]) {
                errors++;
            }
        }
        total_errors += errors;

        // 단어 수 계산
        int words = 1;
        for (int j = 0; sentences[i][j] != '\0'; j++) {
            if (sentences[i][j] == ' ') {
                words++;
            }
        }
        total_words += words;
    }

    // 평균 WPM 계산
    double wpm = (total_words / total_time) * 60;

    // 최종 결과 출력
    printf("\n=== 최종 결과 ===\n");
    printf("총 오류: %d회\n", total_errors);
    printf("평균 분당 타자수: %.2f WPM\n", wpm);

    return 0;
}

