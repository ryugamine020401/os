#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

void execute_command(char **args) {
    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // Child process
        execvp(args[0], args);
        perror("execvp");
        exit(EXIT_FAILURE);
    } else {
        // Parent process
        wait(NULL);
    }
}

void redirect_output(char **args, const char *file, int append) {
    int fd;
    if (append)
        fd = open(file, O_WRONLY | O_CREAT | O_APPEND, 0644);
    else
        fd = open(file, O_WRONLY | O_CREAT | O_TRUNC, 0644);

    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    // Duplicate the file descriptor
    int saved_stdout = dup(STDOUT_FILENO);
    if (saved_stdout == -1) {
        perror("dup");
        exit(EXIT_FAILURE);
    }

    // Redirect stdout to the file
    if (dup2(fd, STDOUT_FILENO) == -1) {
        perror("dup2");
        exit(EXIT_FAILURE);
    }

    // Execute the command
    execute_command(args);

    // Restore stdout
    if (dup2(saved_stdout, STDOUT_FILENO) == -1) {
        perror("dup2");
        exit(EXIT_FAILURE);
    }

    close(saved_stdout);
    close(fd);
}

void redirect_input(char **args, const char *file) {
    int fd = open(file, O_RDONLY);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    // Duplicate the file descriptor
    int saved_stdin = dup(STDIN_FILENO);
    if (saved_stdin == -1) {
        perror("dup");
        exit(EXIT_FAILURE);
    }

    // Redirect stdin to the file
    if (dup2(fd, STDIN_FILENO) == -1) {
        perror("dup2");
        exit(EXIT_FAILURE);
    }

    // Execute the command
    execute_command(args);

    // Restore stdin
    if (dup2(saved_stdin, STDIN_FILENO) == -1) {
        perror("dup2");
        exit(EXIT_FAILURE);
    }

    close(saved_stdin);
    close(fd);
}

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage: %s command\n", argv[0]);
        return 0;
    }

    // Tokenize the command
    char *token;
    char **args = malloc(sizeof(char*) * argc);
    int i = 1; // Start from index 1 to skip program name
    int arg_count = 0;
    while (i < argc) {
        if (strcmp(argv[i], ">") == 0 || strcmp(argv[i], ">>") == 0 || strcmp(argv[i], "<") == 0) {
            i += 2; // Skip the redirection operator and file name
        } else {
            args[arg_count++] = strdup(argv[i++]);
        }
    }
    args[arg_count] = NULL;

    // Check for output redirection
    const char *output_file = NULL;
    int append = 0;
    for (i = 2; i < argc - 1; i++) {
        if (strcmp(argv[i], ">") == 0) {
            output_file = argv[i + 1];
            break;
        } else if (strcmp(argv[i], ">>") == 0) {
            output_file = argv[i + 1];
            append = 1;
            break;
        }
    }

    // Check for input redirection
    const char *input_file = NULL;
    for (i = 2; i < argc - 1; i++) {
        if (strcmp(argv[i], "<") == 0) {
            input_file = argv[i + 1];
            break;
        }
    }

    // Perform the appropriate action based on the redirection type
    if (output_file != NULL) {
        redirect_output(args, output_file, append);
    } else if (input_file != NULL) {
        redirect_input(args, input_file);
    } else {
        // No redirection, execute the command as is
        execute_command(args);
    }

    // Free allocated memory
    for (i = 0; i < arg_count; i++) {
        free(args[i]);
    }
    free(args);

    return 0;
}
