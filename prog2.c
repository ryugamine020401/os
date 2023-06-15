#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

#define MAX_COMMAND_LENGTH 100
#define MAX_ARGS 10

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <command>\n", argv[0]);
        return 1;
    }

    char command[MAX_COMMAND_LENGTH];
    strcpy(command, argv[1]);

    int num_commands = 1;
    char* commands[MAX_ARGS];
    commands[0] = strtok(command, "|");

    while ((commands[num_commands] = strtok(NULL, "|")) != NULL) {
        num_commands++;
    }

    int i;
    int pipefd[2];
    int prev_pipe = 0;

    for (i = 0; i < num_commands; i++) {
        pipe(pipefd);

        pid_t pid = fork();

        if (pid < 0) {
            perror("fork");
            return 1;
        }

        if (pid == 0) {  // Child process
            char* args[MAX_ARGS];
            int arg_count = 0;

            char* token = strtok(commands[i], " ");
            while (token != NULL) {
                args[arg_count++] = token;
                token = strtok(NULL, " ");
            }
            args[arg_count] = NULL;

            if (i > 0) {
                close(STDIN_FILENO);
                dup2(prev_pipe, STDIN_FILENO);
                close(prev_pipe);
            }

            if (i < num_commands - 1) {
                close(STDOUT_FILENO);
                dup2(pipefd[1], STDOUT_FILENO);
                close(pipefd[1]);
            }

            execvp(args[0], args);
            perror("execvp");
            return 1;
        } else {  // Parent process
            close(pipefd[1]);
            prev_pipe = pipefd[0];
        }
    }

    close(prev_pipe);

    for (i = 0; i < num_commands; i++) {
        wait(NULL);
    }

    return 0;
}
