
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>

int main(int argc, char *argv[]) {
    int fd1, fd2, fd3;

    // Open three files
    fd1 = open("file1.txt", O_CREAT | O_WRONLY, 0644);
    fd2 = open("file2.txt", O_CREAT | O_WRONLY, 0644);
    fd3 = open("file3.txt", O_CREAT | O_WRONLY, 0644);

    // Print file descriptors
    printf("File stdin descriptor: %d\n", stdin);
    printf("File stdout descriptor: %d\n", stdout);
    printf("File stderr descriptor: %d\n", stderr);
    printf("File 1 descriptor: %d\n", fd1);
    printf("File 2 descriptor: %d\n", fd2);
    printf("File 3 descriptor: %d\n", fd3);


    // Close files
    close(fd1);
    close(fd2);
    close(fd3);

    return 0;
}
