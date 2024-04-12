#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>

pid_t pid_agent;

void catch_INT(){ // catch Ctrl+C
    kill(pid_agent, SIGTERM);
    exit(0);
}

int main(int argc, char *argv[]) {
    int signal_option;

    // Open file pointer for the agent.pid
    FILE *pid_file_agent = fopen("agent.pid", "r");
    if (pid_file_agent == NULL) {
        perror("Error Reading .pid file");
        exit(1);
    }

    // Read the process ID from agent.pid
    fscanf(pid_file_agent, "%d", &pid_agent);
    fclose(pid_file_agent);

    // Check if the pid_agent is running by sending signal 0
    if (kill(pid_agent, 0) == 0) {
        printf("Agent found.\n");
    } else {
        printf("Error: no agent found.\n");
        exit(1);
    }

    signal(SIGINT, catch_INT); // Ctrol+C Handler

    // Start menu
    while(1){
        printf("\033[2J"); // Clear screen escape sequence 
        printf("\033[H"); // Move cursor to the top left corner
        printf("Choose a signal to send [1: HUP;15: TERM]: \n");
        // Get user option
        scanf("%d", &signal_option);
        while((getchar()) != '\n');

        switch (signal_option)
        {
        case 1: // Send SIGHUP to pid_agent
            kill(pid_agent, SIGHUP);
            break;
        case 15: // Send SIGTERM to pid_agent
            kill(pid_agent, SIGTERM);
            exit(0);
            break;
        default: // Try again...
            printf("Choose an available option...");
            while((getchar()) != '\n');
            break;
        }
    }

    return 0;
}