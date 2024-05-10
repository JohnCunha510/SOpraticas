#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// Function that will be executed by the thread
void *thread_function(void *arg) {
    int *value = (int *)arg; // Cast the void pointer back to int pointer
    int *result = malloc(2 * sizeof(int)); // Allocate memory for the result
    if (result == NULL) {
        perror("Memory allocation failed");
        pthread_exit(NULL); // Exit the thread with failure
    }
    //Perform calculations;
    result[0] = value[0]+value[1];
    result[1] = value[0]*value[1];

    // printf("Received value in thread: %d, %d\n", value[0], value[1]);
    // printf("Send result in thread: %d, %d\n", result[0], result[1]);

    //exit and retun result
    pthread_exit((void *)result);
    return NULL;
}

int main() {
    pthread_t thread; // Declare a variable thread ID
    int number[2];
    void *thread_result;

    //Ask user for 2 integers
    printf("\nChoose two integers:\n");
    scanf("%d", &number[0]);
    scanf("%d", &number[1]);


    // Create a thread
    if (pthread_create(&thread, NULL, thread_function, (void *)&number) != 0) {
        perror("Thread creation failed");
        return EXIT_FAILURE;
    }


    // Wait for the thread to finish and receive value
    if (pthread_join(thread, &thread_result) != 0){
        perror("Thread join failed");
        return EXIT_FAILURE;
    }

    // Cast the void pointer into int pointer.
    // So we can print de result as an integer.
    int *result = (int *)thread_result;
    printf("Thread has completed: %d, %d\n", result[0], result[1]);

    return 0;
}
