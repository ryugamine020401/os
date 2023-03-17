// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <unistd.h>
// #include <sys/wait.h>

// #define MAX_LINE 80 /* The maximum length command */
// #define MAX_ARGS MAX_LINE/2 + 1 /* The maximum number of arguments */

// int main(void) {
//     char input[MAX_LINE]; /* User input */
//     char *args[MAX_ARGS]; /* Command line arguments */
//     int should_run = 1; /* Flag to determine when to exit program */

//     while (should_run) {
//         printf("osh> ");
//         fflush(stdout);

//         /* Read user input */
//         fgets(input, MAX_LINE, stdin);
//         input[strcspn(input, "\n")] = 0; /* Remove trailing newline character */

//         /* Parse input into arguments */
//         char *token = strtok(input, " ");
//         int i = 0;
//         int background = 0;
//         while (token != NULL) {
//             if (strcmp(token, "&") == 0) {
//                 /* Set background flag */
//                 background = 1;
//             } else {
//                 args[i] = token;
//                 i++;
//             }
//             token = strtok(NULL, " ");
//         }
//         args[i] = NULL;

//         /* Fork a child process */
//         pid_t pid = fork();

//         if (pid == 0) {
//             /* Child process */
//             execvp(args[0], args);
//             exit(0); /* Terminate child process */
//         }
//         else if (pid < 0) {
//             /* Forking failed */
//             printf("Forking child process failed!\n");
//             exit(1);
//         }
//         else {
//             /* Parent process */
//             if (!background) {
//                 /* Wait for child process to complete */
//                 wait(NULL);
//             }
//             if (strcmp(args[0], "exit") == 0) {
//                 should_run = 0;
//             }
//         }
//     }

//     return 0;
// }



///////////////////////////////////////////////////////////////////////////////




#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_LINE 80 /* The maximum length command */
#define MAX_ARGS MAX_LINE/2 + 1 /* The maximum number of arguments */

int main(void) {
    char input[MAX_LINE]; /* User input */
    char *args[MAX_ARGS]; /* Command line arguments */
    int should_run = 1; /* Flag to determine when to exit program */

    while (should_run) {
        printf("osh> ");
        fflush(stdout);

        /* Read user input */
        fgets(input, MAX_LINE, stdin);
        input[strcspn(input, "\n")] = 0; /* Remove trailing newline character */

        /* Parse input into arguments */
        char *token = strtok(input, " ");
        int i = 0;
        int background = 0;
        while (token != NULL) {
            if (strcmp(token, "&") == 0) {
                /* Set background flag */
                background = 1;
            } else {
                args[i] = token;
                i++;
            }
            token = strtok(NULL, " ");
        }
        args[i] = NULL;

        /* Fork a child process */
        pid_t pid = fork();

        if (pid == 0) {
            /* Child process */
            execvp(args[0], args);
            exit(0); /* Terminate child process */
        }
        else if (pid < 0) {
            /* Forking failed */
            printf("Forking child process failed!\n");
            exit(1);
        }
        else {
            /* Parent process */
            if (background) {
                /* Background process - don't wait */
                printf("%d\n", pid); /* Print PID of background process */
            } else {
                /* Foreground process - wait for child to complete */
                waitpid(pid, NULL, 0);
                
            }
            if (strcmp(args[0], "exit") == 0) {
                should_run = 0;
            }
        }
        
    }
    return 0;
}
