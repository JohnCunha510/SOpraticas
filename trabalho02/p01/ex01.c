#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define N 1
#define M 16

int msq1_global;
int msq2_global;

void catch_INT(){ // catch Ctrl+C
  // Close the message queue after finishing
  msgctl(msq1, IPC_RMID, NULL);
  msgctl(msq2, IPC_RMID, NULL);
}

// Define a structure for the message
struct msg_buffer {
  long msg_type;
  int msg_data;
};

int main(int argc, char *argv[]) {

  // Create msq1
  key_t key1 = ftok("", 'D'); // Generate a key using ftok()
  int msq1 = msgget(key1, IPC_CREAT | 0666); // Create or obtain a message queue
  if (msq1 == -1) {
      perror("msgget");
      return 1;
  }
  printf("\nmsq1 msg test Start");
  // Create message
        struct msg_buffer message1;
        message1.msg_type = 1; // Message1 type 
        message1.msg_data = 598;

        // Send the message1
        if (msgsnd(msq1, &message1, sizeof(message1.msg_data), 0) == -1) {
            perror("msgsnd");
            return 1;
        }
  // Receive a message1
        struct msg_buffer received_message1;
        if (msgrcv(msq1, &received_message1, sizeof(received_message1.msg_data), 1, 0) == -1) {
            perror("msgrcv");
            return 1;
        }
  printf("\nmsq1 msg test: %d\n", received_message1.msg_data);



  // Create msq2
  key_t key2 = ftok("", 'E'); // Generate a key using ftok()
  int msq2 = msgget(key2, IPC_CREAT | 0666); // Create or obtain a message queue
  if (msq2 == -1) {
      perror("msgget");
      return 1;
  }
  printf("\nmsq2 msg test Start");
  // Create message
        struct msg_buffer message2;
        message2.msg_type = 1; // Message type 
        message2.msg_data = 510;

        // Send the message
        if (msgsnd(msq2, &message2, sizeof(message2.msg_data), 0) == -1) {
            perror("msgsnd");
            return 1;
        }
  // Receive a message
        struct msg_buffer received_message2;
        if (msgrcv(msq2, &received_message2, sizeof(received_message2.msg_data), 1, 0) == -1) {
            perror("msgrcv");
            return 1;
        }
  printf("\nmsq2 msg test: %d\n", received_message2.msg_data);


  for (int i = 0; i < N; i++){
    pid_t pid = fork();
    if (pid == 0) {
      printf("\nIm a child\n");
      // Child process
      int number_key[6]; int j = 0;

      
      
      while(j < 6){
        // Receive a message
        struct msg_buffer received_message;
        if (msgrcv(msq1, &received_message, sizeof(received_message.msg_data), 1, 0) == -1) {
            perror("msgrcv");
            return 1;
        }
        number_key[j] = received_message.msg_data;
        j++;
        printf("\nreceived_message.msg_data: %d, j= %d\n", received_message.msg_data, j);

        /*
        // Create message
        struct msg_buffer message;
        message.msg_type = 1; // Message type (can be any positive integer)
        message.msg_data = 1;

        // Send the message
        if (msgsnd(msq2, &message, sizeof(message.msg_data), 0) == -1) {
            perror("msgsnd");
            return 1;
        }
        */
      }

      //sleep(2);
      printf("\nkey: ");
      for(j = 0; j < 6; j++){
          printf("%d,",number_key[j]);
      }
      /*
      // Create message
        struct msg_buffer message;
        message.msg_type = 1; // Message type (can be any positive integer)
        message.msg_data = 0;

        // Send the message
        if (msgsnd(msq2, &message, sizeof(message.msg_data), 0) == -1) {
            perror("msgsnd");
            return 1;
      }
      */
      exit(0);
    }
  }
  printf("\nIm the parent\n");


  int M_consumers = M; int N_consumers = N;
  

  
        
  while (N_consumers > 0){
    printf("\nN_consumers: %d  |  M_consumers: %d\n", N_consumers, M_consumers);

    while(M_consumers > 0){
      int random_number = 1 + (rand() % 49);  // Generate guess

      // Create message
      struct msg_buffer message;
      message.msg_type = 1; // Message type (can be any positive integer)
      message.msg_data = random_number;

      // Send the message
      if (msgsnd(msq1, &message, sizeof(message.msg_data), 0) == -1) {
          perror("msgsnd");
          return 1;
      }
      M_consumers--;
      printf("\nM_consumers: %d, random_number = %d\n", M_consumers, random_number);

    }

    struct msg_buffer received_message;
    if (msgrcv(msq2, &received_message, sizeof(received_message.msg_data), 1, 0) == -1) {
      perror("msgrcv");
      return 1;
    }
    switch (received_message.msg_data){
    case 1:
      M_consumers++;
      break;
    case 0:
      N_consumers--;
      break;
    default:
      break;
    }    
    
  }

  // Close the message queue after finishing
  msgctl(msq1, IPC_RMID, NULL);
  msgctl(msq2, IPC_RMID, NULL);
  
  return 0;
}