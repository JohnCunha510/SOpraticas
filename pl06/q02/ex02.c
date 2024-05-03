#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>


// Define a structure for the message
struct msg_buffer {
    long msg_type;
    int msg_data;
};

int main(int argc, char *argv[]) {

    key_t key = ftok("tmp", 'A'); // Generate a key using ftok()
    int msgid = msgget(key, IPC_CREAT | 0666); // Create or obtain a message queue

    if (msgid == -1) {
        perror("msgget");
        return 1;
    }

    pid_t pid = fork();
    if (pid == 0) {
        // Child process
        // Generate a random number between 0 and 9
        srand(getpid());
        int secret_number = rand() % 10;

        // Communication loop
        while (1) {
                // Wait for parent's guess
            // Receive a message
            struct msg_buffer received_message;
            if (msgrcv(msgid, &received_message, sizeof(received_message.msg_data), 2, 0) == -1) {
                perror("msgrcv");
                return 1;
            }

            // Prepare a message with type 1 (to be received by the parent)
            struct msg_buffer message;
            message.msg_type = 1; // Message type
            if (secret_number < received_message.msg_data) {
                message.msg_data = 1;
            } else if (secret_number > received_message.msg_data) {
                message.msg_data = -1;
            } else {
                message.msg_data = 0;
            }
            // Send the message to the parent
            if (msgsnd(msgid, &message, sizeof(message.msg_data), 0) == -1) {
                perror("msgsnd");
                return 1;
            }
        }
    } else {
        // Parent process
        // Communication loop
        while (1) {
            int guess = rand() % 10;  // Generate guess
               // Send guess to child
            // Prepare a message
            struct msg_buffer message;
            message.msg_type = 2; // Message type (can be any positive integer)
            message.msg_data = guess;

            // Send the message
            if (msgsnd(msgid, &message, sizeof(message.msg_data), 0) == -1) {
                perror("msgsnd");
                return 1;
            }
            printf("Parent Guessing: %d\n", message.msg_data);  // Display guess

               // Receive response from child
            // Receive a message
            struct msg_buffer received_message;
            if (msgrcv(msgid, &received_message, sizeof(received_message.msg_data), 1, 0) == -1) {
                perror("msgrcv");
                return 1;
            }
            printf("Child Response: %d\n", received_message.msg_data);  // Display response
            if (received_message.msg_data == 0) {
                printf("Correct guess!\n");
                kill(pid, SIGTERM);
                break;  // Exit loop when correct guess is made
            } else if (received_message.msg_data == -1) {
                printf("Guess is too low.\n");
            } else {
                printf("Guess is too high.\n");
            }
        }
    }

    // Close the message queue after finishing
    msgctl(msgid, IPC_RMID, NULL);

    return 0;
}