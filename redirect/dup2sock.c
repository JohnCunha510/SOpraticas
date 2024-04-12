#include <errno.h> 
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/wait.h>

#define SOCKET_PATH "socket"

int main(void) { 

    pid_t childpid = fork(); 
    if (childpid == 0) { // Child process
        int sockfd;
        struct sockaddr_un addr;

        if ((sockfd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
            perror("socket");
            exit(EXIT_FAILURE);
        }

        memset(&addr, 0, sizeof(addr));
        addr.sun_family = AF_UNIX;
        strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

        if (connect(sockfd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
            perror("connect");
            exit(EXIT_FAILURE);
        }
        
        if (dup2(sockfd, STDIN_FILENO) == -1) 
            perror("Failed to redirect stdin of sort");
        else if (close(sockfd) == -1)
            perror("Failed to close extra pipe file descriptors on sort");
        else {
            
        execl("/bin/sort", "sort", "-n", "+4", NULL); 
        perror("Failed to exec sort");
        } 
    }

    //  Parent process
    int sockfd, client_sockfd;
    struct sockaddr_un addr, client_addr;
    socklen_t client_len;

    if ((sockfd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    if (listen(sockfd, 5) == -1) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    client_len = sizeof(client_addr);
    if ((client_sockfd = accept(sockfd, (struct sockaddr *)&client_addr, &client_len)) == -1) {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    if (dup2(client_sockfd, STDOUT_FILENO) == -1) 
        perror("Failed to redirect stdout of ls");
    else if (close(client_sockfd) == -1)
        perror("Failed to close extra pipe descriptors on ls");
    else {

    execl("/bin/ls", "ls", "-l", NULL); 
    perror("Failed to exec ls");
    }

    return 1;
}