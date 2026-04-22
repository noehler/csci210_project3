#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <spawn.h>
#include <sys/wait.h>
#include <unistd.h>

#define N 12
#define MAX_LINE 256
#define MAX_ARGS 21   // 20 args max + NULL terminator

extern char **environ;

char *allowed[N] = {
    "cp", "touch", "mkdir", "ls", "pwd", "cat",
    "grep", "chmod", "diff", "cd", "exit", "help"
};

int isAllowed(const char *cmd) {
    for (int i = 0; i < N; i++) {
        if (strcmp(cmd, allowed[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

void printHelp(void) {
    printf("The allowed commands are:\n");
    for (int i = 0; i < N; i++) {
        printf("%d: %s\n", i + 1, allowed[i]);
    }
}

int main(void) {
    char line[MAX_LINE];

    while (1) {
        fprintf(stderr, "rsh>");

        if (fgets(line, sizeof(line), stdin) == NULL) {
            return 0;
        }

        if (strcmp(line, "\n") == 0) {
            continue;
        }

        line[strcspn(line, "\n")] = '\0';

        char *argv[MAX_ARGS];
        int argc = 0;

        char *token = strtok(line, " \t");
        while (token != NULL && argc < 20) {
            argv[argc++] = token;
            token = strtok(NULL, " \t");
        }
        argv[argc] = NULL;

        if (argc == 0) {
            continue;
        }

        if (!isAllowed(argv[0])) {
            printf("NOT ALLOWED!\n");
            continue;
        }

        if (strcmp(argv[0], "exit") == 0) {
            return 0;
        }

        if (strcmp(argv[0], "help") == 0) {
            printHelp();
            continue;
        }

        if (strcmp(argv[0], "cd") == 0) {
            if (argc > 2) {
                printf("-rsh: cd: too many arguments\n");
                continue;
            }

            if (argc == 1) {
                continue;
            }

            chdir(argv[1]);
            continue;
        }

        pid_t pid;
        int status;

        if (posix_spawnp(&pid, argv[0], NULL, NULL, argv, environ) != 0) {
            perror("posix_spawnp");
            continue;
        }

        waitpid(pid, &status, 0);
    }

    return 0;
}