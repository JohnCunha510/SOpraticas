#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

void child_process(int pipe_in, int pipe_out) {

    // Generate a random number between 0 and 9
    srand(getpid());
    int secret_number = rand() % 10;

    // Communication loop
    while (1) {
        int guess;
        read(pipe_in, &guess, sizeof(guess));  // Wait for parent's guess

        if (guess < secret_number) {
            write(pipe_out, "-1", sizeof("-1"));  // Send response to parent
        } else if (guess > secret_number) {
            write(pipe_out, "1", sizeof("1"));    // Send response to parent
        } else {
            write(pipe_out, "0", sizeof("0"));    // Send response to parent
            break;  // Exit loop when correct guess is made
        }
    }

    close(pipe_in);  // Close unused end of pipes
    close(pipe_out);

    exit(0);
}

void parent_process(int pipe_in, int pipe_out) {

    // Communication loop
    while (1) {
        int guess = rand() % 10;  // Generate guess
        printf("Guessing: %d\n", guess);  // Display guess
        write(pipe_out, &guess, sizeof(guess));  // Send guess to child

        char response[3];
        read(pipe_in, &response, sizeof(response));  // Receive response from child
        printf("Response: %s\n", response);  // Display response
        if (response[0] == '0') {
            printf("Correct guess!\n");
            break;  // Exit loop when correct guess is made
        } else if (response[0] == '-') {
            printf("Guess is too low.\n");
        } else {
            printf("Guess is too high.\n");
        }
    }

    close(pipe_in);  // Close unused end of pipes
    close(pipe_out);

    exit(0);
}

int main() {
    int parent_to_child[2], child_to_parent[2];
    pipe(parent_to_child);
    pipe(child_to_parent);

    pid_t pid = fork();

    if (pid == 0) {
        // Child process
        close(parent_to_child[1]);
        close(child_to_parent[0]);
        child_process(parent_to_child[0], child_to_parent[1]);
    } else {
        // Parent process
        close(parent_to_child[0]);
        close(child_to_parent[1]);
        parent_process(child_to_parent[0], parent_to_child[1]);
    }

    // Close pipes
    close(parent_to_child[0]);
    close(parent_to_child[1]);
    close(child_to_parent[0]);
    close(child_to_parent[1]);

    // Wait for child to terminate
    wait(NULL);

    return 0;
}
