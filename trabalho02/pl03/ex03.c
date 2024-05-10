#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>

// int pointer to receive cast (int *) from thread_result,
// and global for signal handler to use;
int *result; 

void catch_HUP(){
  free(result);
  exit(0);
}
void catch_TERM(){
  free(result);
  exit(0);
}
void catch_INT(){
  free(result);
  exit(0);
}
void catch_QUIT(){
  free(result);
  exit(0);
}
void catch_KILL(){
  free(result);
  exit(0);
}

// Function that will be executed by the thread
void *thread_function(void *arg) {
  int ***table_thread = (int ***)arg; // Cast the void pointer back to int pointer

  int *result = malloc(2 * sizeof(int)); // Allocate memory for the result
  if (result == NULL) {
      perror("Memory allocation failed");
      pthread_exit(NULL); // Exit the thread with failure
  }

  //Perform calculations;
  result[0] = (**table_thread[0]) + (**table_thread[1]);
  result[1] = (**table_thread[0]) * (**table_thread[1]);

  //exit and retun result
  pthread_exit((void *)result);
  return NULL;
}

int main() {
  pthread_t thread; // Declare a variable thread ID
  void *thread_result; // void pointer to receive argument from pthread_join()
  int number1, number2; // Initiate 2 integers

  // Create 2 pointers and link them to each integer
  int *number1_ptr = &number1;
  int *number2_ptr = &number2; 

  // create a table to store the address of de pointers,
  // and send 2 pointers in 1 argument;
  int **table_ptr[2];
  table_ptr[0] = &number1_ptr;
  table_ptr[1] = &number2_ptr;

  //Ask user for 2 integers
  printf("\nChoose two integers:\n");
  scanf("%d", &number1);
  scanf("%d", &number2);

  // Create a thread
  if (pthread_create(&thread, NULL, thread_function, (void *)&table_ptr) != 0) {
    perror("Thread creation failed");
    return 1;
  }

  // Signal Handlers
  signal(SIGHUP, catch_HUP);
  signal(SIGTERM, catch_TERM);
  signal(SIGINT, catch_INT);
  signal(SIGQUIT, catch_QUIT);
  signal(SIGKILL, catch_KILL);

  // Wait for the thread to finish and receive value
  if (pthread_join(thread, &thread_result) != 0){
    perror("Thread join failed");
    return 1;
  }

  // Cast the void pointer into int pointer.
  // So we can print de result as an integer.
  result = (int *)thread_result;
  printf("\nThread has completed: %d, %d\n", result[0], result[1]);

  free(result);

  return 0;
}
