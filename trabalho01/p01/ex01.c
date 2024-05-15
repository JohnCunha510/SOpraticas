#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/wait.h>

#define SOCKET_PATH "pipeline_socket"

int main(int argc, char *argv[]) {

    // Create pipe for pipeline 2 (between cut and sort)
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("Error creating pipe");
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
            perror("find create socket");
            exit(1);
        }
        memset(&addr, 0, sizeof(addr));
        addr.sun_family = AF_UNIX;
        strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);
        // Bind socket
        if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
            perror("find socket bind");
            exit(1);
        }
        // Listen for socket connection
        if (listen(sockfd, 5) == -1) {
            perror("find socket listen");
            exit(1);
        }
        // Accept socket connection
        client_len = sizeof(client_addr);
        if ((client_sockfd = accept(sockfd, (struct sockaddr *)&client_addr, &client_len)) == -1) {
            perror("find socket accept");
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
            perror("Failed to exec find");
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
            perror("Failed to close socket descriptor on cut");
        else {
            // Redirect stdout to pipe[1]
            close(pipefd[0]); // Close read end of pipe
            if (dup2(pipefd[1], STDOUT_FILENO) == -1) // Redirect stdout to pipe
                perror("Failed to redirect stdout of cut");
            else if (close(pipefd[1]) == -1)// Close pipe
                perror("Failed to close pipe descriptor on cut");
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
        // Redirect stdin to pipe[0]
        close(pipefd[1]); // Close read end of pipe
        if (dup2(pipefd[0], STDIN_FILENO) == -1) // Redirect stdin to pipe
            perror("Failed to redirect stdin of sort");
        else if (close(pipefd[0]) == -1)// Close pipe
            perror("Failed to close pipe[0] descriptor on sort");
        else {
            // Open file.txt
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
    close(pipefd[0]);
    close(pipefd[1]);

    // Wait for all child processes to finish
    waitpid(pid_find, NULL, 0);
    waitpid(pid_cut, NULL, 0);
    unlink(SOCKET_PATH);
    waitpid(pid_sort, NULL, 0);


    // Execute less command with input redirection from file.txt
    execlp("less", "less", "file.txt", NULL);
    perror("Error executing less");
    exit(1);

    return 0;
}
/*
linha de comando shell: “find . -type f -ls | cut -c 2- | sort -n -k 7 >file.txt ; less <file.txt”

Esta linha é dividida através de ";" em dois comadandos shell:
    1. find . -type f -ls | cut -c 2- | sort -n -k 7 >file.txt
    2. less <file.txt

No primeiro commando find, cut e sort são ligados através de Pipelines "|"
    "find" procura todos os ficheiros "-type f" na diretoria "." e lista a informação detalhada dos ficheiros "-ls".
    "cut" com "-c 2-" significa que cortamos a linha a partir do segundo caratere até o fim da linha,
ou seja, retira o primeiro caratere de todas as linhas.
    "sort" ordena tododas as linha numericamente "-n", baseado no 7º campo de cada linha
    ">file.txt" redireciona o output de "sort" para o ficheiro "file.txt", se o ficheiro não existir ele cria um

No segundo comado "less" é usado para visualizar o ficheiro "file.txt" no terminal

*/