
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>

int main(int argc, char *argv[]) {
    FILE *fd1, *fd2, *fd3;

    // Open three files
    fd1 = fopen("file1.txt", "w+");
    fd2 = fopen("file2.txt", "w+");
    fd3 = fopen("file3.txt", "w+");

    // Print file descriptors
    printf("File stdin descriptor: %d\n", fileno(stdin));
    printf("File stdout descriptor: %d\n", fileno(stdout));
    printf("File stderr descriptor: %d\n", fileno(stderr));
    printf("File 1 descriptor: %d\n", fileno(fd1));
    printf("File 2 descriptor: %d\n", fileno(fd2));
    printf("File 3 descriptor: %d\n", fileno(fd3));


    // Close files
    fclose(fd1);
    fclose(fd2);
    fclose(fd3);

    return 0;
}
