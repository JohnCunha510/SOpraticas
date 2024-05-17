#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>
#include <time.h>

#define NUM_CONSUMERS 4
#define MAX_NUM 49
#define NUM_PER_KEY 6
#define MESSAGES_PRODUCER 16

// Structure for messages
struct msgbuf {
    long mtype;
    int mvalue;
};

// Global variables
int msq1, msq2;
FILE *output_files[NUM_CONSUMERS];
int *keys_generated[NUM_CONSUMERS];

// Signal handler for cleanup
void signal_handler(int signal) {
for (int i = 0; i < NUM_CONSUMERS; ++i) {
    if (output_files[i] != NULL) {
        fclose(output_files[i]);
        char filename[10];
        sprintf(filename, "Key_%d.txt", i+1);
        remove(filename);
    }
    
    free(keys_generated[i]);

}
msgctl(msq1, IPC_RMID, NULL);
msgctl(msq2, IPC_RMID, NULL);

exit(0);
}

// Function to generate random number between 1 and MAX_NUM
int generate_random() {
    return rand() % MAX_NUM + 1;
}

// Producer function
void producer() {
    // Initialize random seed
    srand(time(NULL));

    // Send M messages to msq2 with mayproduce==1
    struct msgbuf msg;
    msg.mtype = 1;
    msg.mvalue = 1;
    for (int i = 0; i < MESSAGES_PRODUCER; ++i) {
        if (msgsnd(msq2, &msg, sizeof(msg.mvalue), 0) == -1) {
            perror("Producer: Error sending message to msq2");
            exit(1);
        }
    }
    int k=0;

    // Continuously generate random numbers and send to msq1
    while (1) {
        // Wait for mayproduce==1
        if (msgrcv(msq2, &msg, sizeof(msg.mvalue), 0, 0) == -1) {
            perror("Producer: Error receiving message from msq2");
            exit(1);
        }
        if(msg.mvalue == 0){
            k=k+1;

            if(k==4){
                exit(0);
            }
	    }

        // Generate random number
        int random_number = generate_random();
        // Send to msq1
        msg.mtype = 1;
        msg.mvalue = random_number;
        if (msgsnd(msq1, &msg, sizeof(msg.mvalue), 0) == -1) {
            perror("Producer: Error sending message to msq1");
            exit(1);
        }
    }
}

// Consumer function
void consumer(int id) {
    struct msgbuf msg;
    char filename[10];
    sprintf(filename, "Key_%d.txt", id+1);
    output_files[id] = fopen(filename, "w");

    // Initialize array to track generated numbers
    keys_generated[id] = (int *)malloc(NUM_PER_KEY * sizeof(int));
    if (keys_generated[id] == NULL) {
        perror("Failed to allocate memory for keys_generated");
        exit(1);
    }

    for (int i = 0; i < NUM_PER_KEY; ++i) {
        keys_generated[id][i] = 0;
    }

    // Continuously receive messages from msq1
    while (1) {
        // Receive from msq1
        if (msgrcv(msq1, &msg, sizeof(msg.mvalue), 1, 0) == -1) {
            perror("Consumer: Error receiving message from msq1");
            exit(1);
        }

        // Check for repeated number
        int repeated = 0;
        for (int i = 0; i < NUM_PER_KEY; ++i) {
            if (keys_generated[id][i] == msg.mvalue) {
                repeated = 1;
                break;
            }
        }

        // Write to file if not repeated
        if (!repeated) {
            int key_size = 0;
            for (int i = 0; i < NUM_PER_KEY; ++i) {
                if (keys_generated[id][i] != 0) {
                    key_size++;
                }
            }

            if (key_size < NUM_PER_KEY) {
                fprintf(output_files[id], "%d\n", msg.mvalue);
                fflush(output_files[id]);
                keys_generated[id][key_size] = msg.mvalue;
            }
        }

        // Check if key is complete
        int key_complete = 1;
        for (int i = 0; i < NUM_PER_KEY; ++i) {
            if (keys_generated[id][i] == 0) {
                key_complete = 0;
                break;
            }
        }

        // Close file and send mayproduce==0 to msq2 if key is complete
        if (key_complete) {
            fclose(output_files[id]);
            printf("Consumer %d: Key complete. Closing file.\n", id+1);
            msg.mtype = 1;
            msg.mvalue = 0;

            if (msgsnd(msq2, &msg, sizeof(msg.mvalue), 0) == -1) {
                perror("Consumer: Error sending message to msq2");
                exit(1);
            }
            exit(0);
        }

        // Send mayproduce==1 to msq2
        msg.mtype = 1;
        msg.mvalue = 1;
        if (msgsnd(msq2, &msg, sizeof(msg.mvalue), 0) == -1) {
            perror("Consumer: Error sending message to msq2");
            exit(1);
        }
    }
}

int main() {
    // Create message queues
    msq1 = msgget(IPC_PRIVATE, IPC_CREAT | 0666);
    msq2 = msgget(IPC_PRIVATE, IPC_CREAT | 0666);

    // Set up signal handlers
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    // Create producer process
    pid_t pid = fork();
    if (pid == 0) {
        producer();
    } else if (pid > 0) {
        // Create consumer processes
        for (int i = 0; i < NUM_CONSUMERS; ++i) {
            pid_t consumer_pid = fork();
            if (consumer_pid == 0) {
                consumer(i);
            } else if (consumer_pid < 0) {
                perror("Error forking consumer process");
                exit(1);
            }
        }

        // Wait for all consumers to terminate
        int status;
        for (int i = 0; i < NUM_CONSUMERS; ++i) {
            wait(&status);
        }

        // Terminate program
        signal_handler(SIGINT);

    } else {
        perror("Error forking producer process");
        exit(1);
    }

    return 0;

}