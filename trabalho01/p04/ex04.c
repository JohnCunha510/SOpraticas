#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>

#define MAXBUF 10

int main(int argc, char *argv[]) {
    int table[MAXBUF];
    int i;

    // Fill table
    for(i = 0; i < MAXBUF; i++){
        table[i] = i+1;
    }

    // Open file pointer for filetable.bin
    int filetable_file = open("filetable.bin" , O_WRONLY | O_CREAT, 0666);
    if (filetable_file == -1) {
        perror("Error Opening filetable.bin");
        exit(1);
    }
    // Print table to filetable.bin
    for(i = 0; i < MAXBUF; i++){
        write(filetable_file, &table[i], sizeof(table[i]));
    }
    close(filetable_file);

    // Fork child
    pid_t pid_num = fork();
    if (pid_num == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    if (pid_num == 0) {  // Child process
        pid_t parent_pid = getppid();
        int child_table[MAXBUF];

        // Open file pointer for filetable.bin
        int filetable_child = open("filetable.bin" ,O_RDONLY);
        if (filetable_child == -1) {
            perror("Error Readind filetable.bin");
            exit(1);
        }
        // Print table to filetable.bin
        for(i = 0; i < MAXBUF; i++){
            read(filetable_child, &child_table[i], sizeof(child_table[i]));
        }
        close(filetable_child);

        // Print table to terminal
        for(i = 0; i < MAXBUF; i++){
        printf("table[%d] = %d\n", i ,child_table[i]);
        }

        while (getppid() == parent_pid){} // wait for parent process to terminate
        printf("\nThe child process is terminating.\n\n");
        exit(0);

    }else {  // Parent process
        sleep(2);
        printf("\nThe parent process is terminating.\n\n");
        exit(0);
    }


    return 0;
}