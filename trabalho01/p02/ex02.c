#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/wait.h>

#define SOCKET_PATH "pipeline_socket"
#define FIFO_PATH "pipeline_fifo"

int main(int argc, char *argv[]) {

    // Create named pipe for pipeline 2 (between cut and sort)
    if (mkfifo(FIFO_PATH, 0666) == -1) {
        perror("Error creating FIFO");
        exit(1);
    }

    // Fork child find
    pid_t pid_find = fork();
    if (pid_find == -1) {
        perror("fork child find");
        exit(1);
    }
    if (pid_find == 0) { // Child find process
        int sockfd, client_sockfd;
        struct sockaddr_un addr, client_addr;
        socklen_t client_len;
        // Create socket
        if ((sockfd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
            perror("socket");
            exit(1);
        }
        memset(&addr, 0, sizeof(addr));
        addr.sun_family = AF_UNIX;
        strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);
        // Bind socket
        if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
            perror("bind");
            exit(1);
        }
        // Listen for socket connection
        if (listen(sockfd, 5) == -1) {
            perror("listen");
            exit(1);
        }
        // Accept socket connection
        client_len = sizeof(client_addr);
        if ((client_sockfd = accept(sockfd, (struct sockaddr *)&client_addr, &client_len)) == -1) {
            perror("accept");
            exit(1);
        }
        // Redirect stdout to socket
        if (dup2(client_sockfd, STDOUT_FILENO) == -1) // Redirect stdout to socket
            perror("Failed to redirect stdout of find");
        else if (close(client_sockfd) == -1) // Close socket
            perror("Failed to close socket descriptor on find");
        else {
            // Execute find
            execlp("find", "find", ".", "-type", "f", "-ls", NULL);
            perror("Failed to exec ls");
        }
        exit(1);
    }

    // Fork child cut
    pid_t pid_cut = fork();
    if (pid_cut == -1) {
        perror("fork child cut");
        exit(1);
    }
    if (pid_cut == 0) {  // Child cut process
        int sockfd;
        struct sockaddr_un addr;
        // Create socket
        if ((sockfd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
            perror("cut socket create");
            exit(1);
        }
        memset(&addr, 0, sizeof(addr));
        addr.sun_family = AF_UNIX;
        strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);
        // Connect to the socket
        usleep(100*1000); // wait a bit til de socket is created in find
        if (connect(sockfd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
            perror("cut socket connect");
            exit(1);
        }
        // Redirect stdin to socket
        if (dup2(sockfd, STDIN_FILENO) == -1) // Redirect stdin to socket
            perror("Failed to redirect stdin of cut");
        else if (close(sockfd) == -1) // Close socket
            perror("Failed to close socket descriptors on cut");
        else {
            // Open FIFO to write
            int fd = open(FIFO_PATH, O_WRONLY);
            if (fd == -1) {
                perror("Error opening FIFO");
                exit(1);
            }
            // Redirect stdout to FIFO
            if (dup2(fd, STDOUT_FILENO) == -1) // Redirect stdout to FIFO
                perror("Failed to redirect stdout of cut");
            else if (close(fd) == -1)// Close FIFO
                perror("Failed to close FIFO descriptors on cut");
            else {
                // Execute cut
                execlp("cut", "cut", "-c", "2-", NULL);
                perror("Error executing cut");
            } 
        }
        exit(1);
    }

    // Fork child sort
    pid_t pid_sort = fork();
    if (pid_sort == -1) {
        perror("fork child sort");
        exit(1);
    }
    if (pid_sort == 0) {  // Child sort process
        // Open FIFO to read
        int fd_fifo = open(FIFO_PATH, O_RDONLY);
        if (fd_fifo == -1) {
            perror("Error opening FIFO");
            exit(1);
        }
        // Redirect stdin to FIFO
        if (dup2(fd_fifo, STDIN_FILENO) == -1) // Redirect stdin to FIFO
            perror("Failed to redirect stdin of sort");
        else if (close(fd_fifo) == -1)// Close FIFO
            perror("Failed to close FIFO descriptor on sort");
        else {
            // // Open file.txt
            int fd_text = open("file.txt", O_WRONLY | O_CREAT , 0644);
            if (fd_text == -1) {
                perror("Error opening file");
                exit(1);
            }
            // Redirect stdout to file.txt
            if (dup2(fd_text, STDOUT_FILENO) == -1) // Redirect stdout to file.txt
                perror("Failed to redirect stdout of sort");
            else if (close(fd_text) == -1)// Close file.txt descriptor
                perror("Failed to close file.txt descriptor on sort");
            else {
                // Execute sort
                execlp("sort", "sort", "-n", "-k", "7", NULL);
                perror("Error executing sort");
            }
        }
        exit(1);
    }

    // Wait for all child processes to finish
    waitpid(pid_find, NULL, 0);
    waitpid(pid_cut, NULL, 0);
    unlink(SOCKET_PATH);
    waitpid(pid_sort, NULL, 0);
    unlink(FIFO_PATH);

    // Execute less command with input redirection from file.txt
    execlp("less", "less", "file.txt", NULL);
    perror("Error executing less");
    exit(1);

    return 0;
}