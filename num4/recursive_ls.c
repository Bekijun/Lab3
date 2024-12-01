#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>

void list_directory(const char *path) {
    struct dirent *entry;
    DIR *dp = opendir(path);

    if (dp == NULL) {
        perror("opendir");
        return;
    }

    printf("\n%s:\n", path);
    
    // 디렉토리 내의 모든 항목 나열
    while ((entry = readdir(dp)) != NULL) {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            printf("%s\n", entry->d_name);
        }
    }

    // 디렉토리를 다시 열어서 재귀적으로 탐색
    rewinddir(dp);
    while ((entry = readdir(dp)) != NULL) {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            char full_path[1024];
            snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);

            struct stat statbuf;
            if (stat(full_path, &statbuf) == 0 && S_ISDIR(statbuf.st_mode)) {
                list_directory(full_path);
            }
        }
    }

    closedir(dp);
}

int main(int argc, char *argv[]) {
    const char *start_path = (argc > 1) ? argv[1] : "."; // 기본 경로는 현재 디렉토리
    list_directory(start_path);
    return 0;
}

