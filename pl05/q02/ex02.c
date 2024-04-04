#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    int pipefd[2];
    pid_t pid_ls, pid_wc;
    int status;

    // Create pipe
    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    // Fork first child (ls)
    printf("Child ls created.\n");
    pid_ls = fork();
    if (pid_ls == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid_ls == 0) {  // Child process (ls)
        // Close read end of the pipe
        close(pipefd[0]);

        // Redirect stdout to write end of the pipe
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);

        // Execute 'ls' command
        execlp("ls", "ls", NULL);
        perror("execlp(ls)");
        exit(EXIT_FAILURE);
    } else {  // Parent process
        // Fork second child (wc -l)
        printf("Child wc created.\n");
        pid_wc = fork();
        if (pid_wc == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        }

        if (pid_wc == 0) {  // Child process (wc -l)
            // Close write end of the pipe
            close(pipefd[1]);

            // Redirect stdin to read end of the pipe
            dup2(pipefd[0], STDIN_FILENO);
            close(pipefd[0]);

            // Redirect stdout to count.txt
            FILE *output_file = fopen("count.txt", "w");
            if (output_file == NULL) {
                perror("fopen");
                exit(EXIT_FAILURE);
            }
            dup2(fileno(output_file), STDOUT_FILENO);

            // Execute 'wc -l' command
            execlp("wc", "wc", "-l", NULL);
            perror("execlp(wc)");
            exit(EXIT_FAILURE);
        } else {  // Parent process
            // Close both ends of the pipe
            close(pipefd[0]);
            close(pipefd[1]);

            // Wait for child processes to terminate
            waitpid(pid_ls, &status, 0);
            printf("Child ls terminated.\n");
            waitpid(pid_wc, &status, 0);
            printf("Child wc terminated.\n");

        }
    }

    return 0;
}
