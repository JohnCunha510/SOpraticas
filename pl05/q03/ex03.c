#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>

#define FIFO_NAME "communication_fifo"

int main() {
    pid_t pid;
    int fd;
    int guess, number, result;
    char buffer[2];

    // Create the FIFO
    mkfifo(FIFO_NAME, 0666);

    // Fork a child process
    pid = fork();

    if (pid < 0) {
        perror("Fork failed");
        exit(1);
    } else if (pid == 0) { // Child process
        // Generate a random number between 0 and 9
        srand(getpid());
        int secret_number = rand() % 10;
        
        // Open FIFO for writing
        fd = open(FIFO_NAME, O_RDWR);
        if (fd == -1) {
            perror("Child: Error opening FIFO for writing");
            exit(1);
        }

        while (1) {
            // Read guess from parent
            read(fd, &guess, sizeof(guess));
            printf("Number Guess: %d\n", guess);
            printf("Secret_number: %d\n", secret_number);

            // Check guess against number
            if (guess == secret_number) {
                // Send hit message to parent
                buffer[0] = '0'; // Hit
                write(fd, buffer, sizeof(buffer));
                close(fd);
                break;
            } else if (guess < number) {
                // Send below message to parent
                buffer[0] = '-'; // Below
                write(fd, buffer, sizeof(buffer));
            } else {
                // Send above message to parent
                buffer[0] = '+'; // Above
                write(fd, buffer, sizeof(buffer));
            }
        }
    } else { // Parent process
        // Open FIFO for reading
        fd = open(FIFO_NAME, O_RDONLY);
        if (fd == -1) {
            perror("Parent: Error opening FIFO for reading");
            exit(1);
        }
        // Seed random number generator
            srand(time(NULL));

        while (1) {
            
            // Generate a random number between 0 and 9
            number = rand() % 10;

            // Send guess to child
            write(fd, &number, sizeof(number));
            printf("Guess: %d\n", number);

            // Read response from child
            read(fd, buffer, sizeof(buffer));

            // Check response
            result = buffer[0];
            if (result == '0') {
                printf("Congratulations! You guessed the number.\n");
                break;
            } else if (result == '-') {
                printf("The number is higher.\n");
            } else if (result == '+') {
                printf("The number is lower.\n");
            }
        }

        // Close FIFO
        close(fd);
        // Remove FIFO
        unlink(FIFO_NAME);
    }

    return 0;
}
