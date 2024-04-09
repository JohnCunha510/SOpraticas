#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#define FIFO_PATH "my_fifo"

int main() {
    pid_t pid;
    int fifo_fd;
    char buffer[50];

    // Create FIFO
    if (mkfifo(FIFO_PATH, 0666) == -1) {
        perror("mkfifo");
        exit(EXIT_FAILURE);
    }

    // Fork
    pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    // Child process
    if (pid == 0) {
        while (1) {
            fifo_fd = open(FIFO_PATH, O_RDONLY);
            if (fifo_fd == -1) {
                perror("open");
                exit(EXIT_FAILURE);
            }
            read(fifo_fd, buffer, sizeof(buffer));
            printf("Child received: %s\n", buffer);
            close(fifo_fd);

            fifo_fd = open(FIFO_PATH, O_WRONLY);
            if (fifo_fd == -1) {
                perror("open");
                exit(EXIT_FAILURE);
            }
            write(fifo_fd, "Message from child", strlen("Message from child") + 1);
            close(fifo_fd);

            printf("Parent received: %s\n", buffer);
        }
        close(fifo_fd);
    } 
    // Parent process
    else {
        // Open FIFO for reading and writing
        // Write to FIFO and wait for reply
        while (1) {
            fifo_fd = open(FIFO_PATH, O_WRONLY);
            if (fifo_fd == -1) {
                perror("open");
                exit(EXIT_FAILURE);
            }
            write(fifo_fd, "Message from parent", strlen("Message from parent") + 1);
            close(fifo_fd);

            fifo_fd = open(FIFO_PATH, O_RDONLY);
            if (fifo_fd == -1) {
                perror("open");
                exit(EXIT_FAILURE);
            }
            read(fifo_fd, buffer, sizeof(buffer));
            close(fifo_fd);

            printf("Parent received: %s\n", buffer);
        }
    }

    close(fifo_fd); // Close FIFO (both processes will terminate)

    return 0;
}
